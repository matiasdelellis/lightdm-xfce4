#!/bin/sh
# Run this to generate all the initial makefiles, etc.

PKG_NAME="lightdm-xfce4-greeter"

echo "Running Xfce Developer Tools..."

xdt-autogen $@
if [ $? -ne 0 ]; then
  echo "xdt-autogen Failed"
  echo "Prease, install xfce4-dev-tools"
  echo "or verify Errors"
fi
