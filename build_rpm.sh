#!/bin/sh

# SPDX-FileCopyrightText: 2025-2026 Erin Catto
# SPDX-License-Identifier: MIT

# Exit on error
set -e

# Get version slug
package_name=box3d
version_hash=$(git describe --tags --match "v*" | sed 's/^v//' | sed 's/-/./g')
echo "** repository hash: ${version_hash} ..."

# Create dir tree for rpmbuild in user dir
rpmdev-setuptree

# Archive repository
(git archive --format=tar.gz --prefix=${package_name}-${version_hash}/ -o ~/rpmbuild/SOURCES/${package_name}-${version_hash}.tar.gz HEAD)
echo "** created archive: ~/rpmbuild/SOURCES/${package_name}-${version_hash}.tar.gz"
sleep 1

# Replace spec version
sed -i "s/Version:.\+/Version: ${version_hash}/g" package/rpm/${package_name}.spec
echo "** building package version: ${version_hash}"
sleep 1

# Check dependencies
echo "** Build dependency check (shown as message only):"
echo "   sudo dnf builddep -y package/rpm/${package_name}.spec"
echo "** Please run the command above manually if build dependencies are missing."
sleep 2

# Build package, let it automatically download extra sources
rpmbuild --define "debug_package %{nil}" --undefine "_disable_source_fetch" --clean -bb package/rpm/${package_name}.spec

echo "** packages for ${package_name}-${version_hash} complete:"
ls ~/rpmbuild/RPMS/$(uname -m)/${package_name}-*${version_hash}*.rpm | cat
ls ~/rpmbuild/RPMS/noarch/${package_name}-*${version_hash}*.rpm | cat
