Testing concepts required for deployable Linux Desktop Apps

NOTE:
---
isandbox is used to launch idemoapp in a read only bind-mounted root
file system (via pivot_root) and drops privileges back to the calling user,
taking advantage of the setuid bits to accomplish its mission.

The app will use a GtkFileChooser within its own process to
choose files, and also open an out of process file chooser to open a file
from the host using IPortal, with the IPortal framework having the appropriate
IPortalProvider.

Note that nasty double-forkers may bomb the sandbox right now, but they
die and so does the sandbox. Just ensure that everything in jail/ is
umount'ed correctly if this happens.

Build Instructions
---

    autoreconf -vfi
    ./configure --prefix=/usr
    make
    sudo make install
    iportald &
    mkdir jail
    isandbox


Authors
---
 * Ikey Doherty <ikey.doherty@gmail.com>
