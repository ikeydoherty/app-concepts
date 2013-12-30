/*
 * sandbox.c - Testing app sandboxing
 *
 * NOTE: This currently hard-codes gedit as the app to launch, exposes
 * /run/ to the client application and requires being run setuid.
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sched.h>
#include <wait.h>
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define JAIL "./jail"

/** Free a pointer, because GCC incorrectly detects free on non-heap */
static inline void freep(void *d) {
        free(*(void**)d);
}

/* Helpful macro to cleanup automatically */
#define __cleanup__ __attribute__ ((cleanup(freep)))

/**
 * Determine if two strings are equal
 * @param x first string to test
 * @param y second string to test
 * @return true or false, depending on whether the strings match
 */
#define streq(x,y) strcmp(x,y) == 0

/**
 * Iterate a null terminated array
 */
#define foreach_string(x,y,i) char *y = NULL;int i;\
         for (i=0; (y = *(x+i)) != NULL; i++ )

/**
 * Check if a string is in an array
 * @param array Array to check
 * @param key Key to check is equal
 * @return true if the key was found
 */
static inline bool str_in_array(char **array, char *key)
{
        foreach_string(array, kTmp, i) {
                if (streq(kTmp, key))
                        return true;
        }
        return false;
}

/**
 * Retrieve home directory for the given user
 * @param uid User ID
 * @return home directory (private owned)
 */
static const char *get_home_directory(uid_t uid)
{
        struct passwd *pw  = NULL;

        pw = getpwuid(uid);
        if (!pw)
                return NULL;
        return pw->pw_dir;
}

/* Environmental variables */
extern char **environ;

/** List of permitted environmental variables to pass to child */
static char *permitted_vars[] = {
                "DISPLAY",
                "HOME",
                "LANG",
                "XDG_RUNTIME_DIR",
                "DBUS_SESSION_BUS_ADDRESS",
                "LIBGL_DEBUG",/**< Temporary access for GL debugging */
                NULL
        };

char** cleaned_env(void)
{
        size_t i = 0, index = 0, length = 0;
        char *key = NULL;
        char **array = NULL;
        char *clone = NULL;

        array = malloc(sizeof(char**));
        if (!array) {
                fprintf(stderr, "Unable to allocate memory: %s\n", strerror(errno));
                return NULL;
        }
        /* Searching environment */
        foreach_string(environ, kTmp, j) {
                clone = strdup(kTmp);
                key = strtok(clone, "=");
                /* If permitted, copy this permitted <K,V> pair into the new array */
                if (str_in_array(permitted_vars, key)) {
                        array = (char**)realloc(array, (sizeof(char**))*length+1);
                        if (!array) {
                                fprintf(stderr, "Memory failure: %s\n", strerror(errno));
                                return NULL;
                        }
                        array[index] = kTmp;
                        index++;
                        length = index + 1;
                }
                i++;
                free(clone);
        }
        if (length > 0) {
                /* Null terminate this array */
                array = realloc(array, (sizeof(char*))*index+1);
                if (!array) {
                        fprintf(stderr, "Memory failure: %s\n", strerror(errno));
                        return NULL;
                }
                length++;
                array[index] = NULL;
        }
        return array;
}

int main(int argc, char **argv)
{
        printf("Running execution\n");
        int rc, status, child_pid, wpid;
        char *name = "gedit";
        char *args[] = { name, NULL };
        __cleanup__ char **envp = NULL;
        __cleanup__ char *work_dir = NULL;
        __cleanup__ char *tmpfs_home = NULL;
        __cleanup__ char *proc_dir = NULL;
        __cleanup__ char *tmp_dir = NULL;
        __cleanup__ char *run_dir = NULL;
        const char *home_dir = NULL;
        uid_t uid, euid;
        gid_t gid, egid;

        /* We use the callers uid and gid for execution */
        uid = getuid();
        gid = getgid();

        home_dir = get_home_directory(uid);
        if (!home_dir) {
                fprintf(stderr, "Failed to determine home directory\n");
                return EXIT_FAILURE;
        }

        envp = cleaned_env();

        /* Determine current access */
        euid = geteuid();
        egid = getegid();
        if (euid != 0 || euid == uid) {
                fprintf(stderr, "Unable to launch with UID: %d\n", euid);
                return EXIT_FAILURE;
        }
        if (egid != 0 || egid == gid) {
                fprintf(stderr, "Unable to launch with GID: %d\n", egid);
                return EXIT_FAILURE;
        }

        /* Deal with the absolute path only */
        work_dir = realpath(JAIL, NULL);
        if (!work_dir) {
                fprintf(stderr, "Unable to obtain real path\n");
                return EXIT_FAILURE;
        }
        fprintf(stdout, "Work dir is: %s\n", work_dir);

        /* Construct fake home directory mount */
        asprintf(&tmpfs_home, "%s/home", work_dir);
        if (!tmpfs_home) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                return EXIT_FAILURE;
        }
        fprintf(stdout, "Home dir is: %s\n", tmpfs_home);

        /* Construct a path for the procfs */
        asprintf(&proc_dir, "%s/proc", work_dir);
        if (!proc_dir) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Construct a path for the /tmp fs */
        asprintf(&tmp_dir, "%s/tmp", work_dir);
        if (!tmp_dir) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Construct a path for the /run fs */
        asprintf(&run_dir, "%s/run", work_dir);
        if (!run_dir) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Unshare FS and PID */
        if ((rc = unshare(CLONE_NEWNS | CLONE_NEWPID)) < 0) {
                fprintf(stderr, "Unable to unshare! %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Bind mount root file system */
        if ((rc = mount("/", work_dir, "", MS_BIND, "") < 0)) {
                fprintf(stderr, "Unable to mount! %s\n", strerror(errno));
                return EXIT_FAILURE;
        }
        /* Bind remount read only root file system */
        if ((rc = mount("/", work_dir, "",
                MS_BIND | MS_REMOUNT| MS_RDONLY, "")) < 0) {
                fprintf(stderr, "Unable to remount! %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Bind mount run dir */
        if ((rc = mount("/run", run_dir, "", MS_BIND, "")) < 0) {
                fprintf(stderr, "Unable to bind-mount /run: %s\n", strerror(errno));
                return EXIT_FAILURE;
        }

        /* Fork to exec */
        if ((child_pid = fork()) == 0) {
                /* Enter the jail */
                if ((rc = chdir(work_dir)) < 0) {
                        fprintf(stderr, "Unable to chdir! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = chroot(work_dir)) < 0) {
                        fprintf(stderr, "Unable to chroot! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = mount("proc", "proc", "proc",
                        0, "")) < 0) {
                        fprintf(stderr, "Unable to mount proc! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = mount("none", "/tmp", "tmpfs",
                        0, "")) < 0) {
                        fprintf(stderr, "Unable to mount tmp! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = mount("none", "/home/", "tmpfs",
                        MS_NOEXEC | MS_NOATIME, "")) < 0) {
                        fprintf(stderr, "Unable to mount home! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = mkdir(home_dir, 0755)) < 0) {
                        fprintf(stderr, "Unable to create home directory! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                if ((rc = chown(home_dir, uid, gid)) < 0) {
                        fprintf(stderr, "Unable to chown to user! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }

                /* Cleanup required in fork because of execvpe */
                free(work_dir);
                free(tmpfs_home);
                free(proc_dir);

                /* Drop all permissions and become calling user */
                seteuid(uid);
                setegid(gid);

                /* Replace ourselves with the new process */
                if ((rc = execvpe(name, args, envp)) < 0) {
                        fprintf(stderr, "Error launching: %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
        } else {
                while ((wpid = wait(&status)) > 0) {
                        continue;
                }
                fprintf(stdout, "Child finished\n");
                /* Unmount the temporary home */
                if ((rc = umount2(tmpfs_home, MNT_FORCE)) < 0) {
                        fprintf(stderr, "Error! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                /* Unmount the temporary proc */
                if ((rc = umount2(proc_dir, MNT_FORCE)) < 0) {
                        fprintf(stderr, "Error! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                /* Unmount the temporary .. /tmp */
                if ((rc = umount2(tmp_dir, MNT_FORCE)) < 0) {
                        fprintf(stderr, "Error! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                /* Unmount the temporary /run */
                if ((rc = umount2(run_dir, MNT_DETACH)) < 0) {
                        fprintf(stderr, "Error! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
                /* Detach the bind point */
                if ((rc = umount2(work_dir, MNT_DETACH)) < 0) {
                        fprintf(stderr, "Error! %s\n", strerror(errno));
                        return EXIT_FAILURE;
                }
        }
        printf("Left execution\n");
	return 0;
}

