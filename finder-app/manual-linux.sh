#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    # JDH next step : deep clean
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper	# JDH lecture slide

    # JDH next step : configure our virt ARM dev board we will simulate in QEMU 
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig	# JDH lecture slide
 
    # JDH next step : build our vmlinux target
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all	# JDH build a kernel image for booting with QEMU
 
    # JDH final step : build the modules and device tree
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs	# JDH lecture slide : devicetree
fi

echo "Adding the Image in outdir"
    # JDH copy ~something~ to "$OUTDIR"		# JDH figure out the results above and then copy
    cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
# JDH : appears to be 1.e
mkdir ${OUTDIR}/rootfs	# JDH add
cd rootfs		# JDH add
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var	# JDH added
mkdir -p usr/bin usr/lib usr/sbin				# JDH added
mkdir -p /var/log						# JDH added

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean	# JDH added
    make defconfig	# JDH added
else
    cd busybox
fi

# TODO: Make and install busybox
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} # JDH added
    make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install # JDH added

cd "$OUTDIR"/rootfs	# JDH but why does ..else.. above cd to busybox?

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)  # JDH find the base location for files to copy

# TODO: Add library dependencies to rootfs
cp ${SYSROOT}/lib/ld-linux-aarch64.so.1 lib # JDH : SYSROOT is key : Git Actions uses containers
cp ${SYSROOT}/lib64/libm.so.6 lib64 
cp ${SYSROOT}/lib64/libresolv.so.2 lib64
cp ${SYSROOT}/lib64/libc.so.6 lib64

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3 # JDH added : null device
sudo mknod -m 666 dev/console c 5 1 # JDH added : console device

# TODO: Clean and build the writer utility
cd ${FINDER_APP_DIR}	# get to the location where we can build Assignment 2 => the calling dir for this script
make clean	# JDH added : clean Assignment 2
make CROSS_COMPILE=${CROSS_COMPILE}

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cd ${FINDER_APP_DIR}	# get to the location where we can build Assignment 2 => the calling dir for this script
cp writer ${OUTDIR}/rootfs/home	# JDH start by trying this as home directory
cp finder.sh ${OUTDIR}/rootfs/home			# JDH
mkdir ${OUTDIR}/rootfs/home/conf			# JDH SUN : but it's buffalo not root : run chown?
cp conf/username.txt ${OUTDIR}/rootfs/home/conf/.	# JDH check if this puts it in /home or /home/conf
cp conf/assignment.txt ${OUTDIR}/rootfs/home/conf/.	# JDH " 
cp finder-test.sh ${OUTDIR}/rootfs/home			# JDH
# JDH this is not in the TODO above but in the instructions to copy at some point. Here seems good:
cp autorun-qemu.sh ${OUTDIR}/rootfs/home		# JDH not in this TODO but mentioned in instructions

# TODO: Chown the root directory
#echo "## Chown the root directory ###" # JDH added
sudo chown root:root ${OUTDIR}/rootfs	# JDH let's try this

# TODO: Create initramfs.cpio.gz
cd "${OUTDIR}/rootfs"	# JDH from slide
sudo chown -R root:root .  # JDH SUN night : let's set them all to root and see...
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio # JDH from slide
cd ${OUTDIR}	# JDH from slide
gzip -f initramfs.cpio	# JDH from slide
sudo chown root:root initramfs.cpio.gz # JDH SUN
