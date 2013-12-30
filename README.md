Testing concepts required for deployable Linux Desktop Apps

NOTE:
---
isandbox is used to launch idemoapp in a read only bind-mounted root
file system (chroot'd) and drops privileges back to the calling user,
taking advantage of the setuid bits to accomplish its mission.

idemoapp is currently doing nothing. The idea is to flesh out IPortal
framework. The app will use a GtkFileChooser within its own process to
choose files, and also open an out of process file chooser to open a file
from the host using IPortal, with the IPortal framework having the appropriate
IPortalProvider.

Note, however, that part isn't implemented yet. All you get is IDemoApp
in ISandbox, doing nothing. If you wish to test, note that it requires
a "jail" directory to exist in your working directory. This will become
configurable in the future. Changing the "name" string within sandbox.c
will allow you to test other applications.

Note that nasty double-forkers may bomb the sandbox right now, but they
die and so does the sandbox. Just ensure that everything in jail/ is
umount'ed correctly if this happens.

Authors
---
 * Ikey Doherty <ikey.doherty@gmail.com>
