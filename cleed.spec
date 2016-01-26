###
### RPM spec file for CLEED
###
### Adapted from jEdit's spec file http://www.jedit.org
###
Summary: Computational Low Energy Electron Diffraction
Name: cleed
Provides: cleed
Version: 0.1
Release: 1
License: GPL
Group: Applications/Engineering/
Source0: %{name}-%{version}.tar.bz2
URL: https://bitbucket.org/cleed/cleed/overview
Vendor: Liam Deacon <liam.m.deacon@gmail.com>
Packager: Liam Deacon <liam.m.deacon@gmail.com>
BuildArch: i386
BuildRoot: %{_builddir}/%{name}-root
Requires: libtiff, libjpeg, libpng
BuildRequires: libtiff-devel
Recommends: blas-devel, lapack-devel, cairo-devel, gsl-devel

%description
CLEED is a computational package for Low Energy Electron Diffraction (LEED) 
IV analysis. It fits experimental IV data curves with those simulated by the 
cleed program and a 'goodness' of fit is made with the R-Factor program 
crfac. Optimisation of the input parameters is performed by csearch.

%prep
%setup -n %{name}-%{version}

%build
./configure --prefix=/usr
make

%install
make DESTDIR=%{buildroot} install

%clean
rm -rf %{buildroot}

%files
/usr/bin/cleed
/usr/bin/crfac
/usr/bin/csearch
/usr/bin/ftsmooth
/usr/bin/latt
/usr/bin/patt
/usr/share/doc/cleed/AUTHORS
/usr/share/doc/cleed/COPYING
/usr/share/doc/cleed/INSTALL
/usr/share/doc/cleed/NEWS
/usr/share/doc/cleed/README
/usr/share/doc/cleed/TODO