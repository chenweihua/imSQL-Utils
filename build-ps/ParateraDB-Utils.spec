Name:       ParateraDB-Utils	
Version:    1.0.1
Release:	1%{?dist}
Summary:	ParateraDB Utils Tools.

Group:      Databases/ParateraDB	
License:    Copyright(c) 2015	
URL:	    https://git.paratera.net/dba/ParateraDB-Utils	
Source0:    https://git.paratera.net/dba/%{name}-%{version}.tar.gz	


BuildRequires: coreutils grep procps shadow-utils gcc-c++ gperf ncurses-devel perl readline-devel time zlib-devel libaio-devel bison cmake make automake	
Requires: mailx tar openssl grep coreutils procps shadow-utils perl time pigz ParateraDB-Backup ParateraDB-Server-server-56 ParateraDB-Server-client-56	

%description
    ParateraDB Utils Tools.

%prep
%setup -q


%build
make %{?_smp_mflags}


%install

RBR=$RPM_BUILD_ROOT
MBD=$RPM_BUILD_DIR/%{name}-%{version}

install -D -m 0755 $MBD/read_metadata $RBR/%{_bindir}/read_metadata
install -D -m 0755 $MBD/registe_metadata $RBR/%{_bindir}/registe_metadata
install -D -m 0755 $MBD/support-scripts/DatabaseBackup.sh $RBR/%{_bindir}/DatabaseBackup
install -D -m 0755 $MBD/support-scripts/CompressDbxBak.sh $RBR/%{_bindir}/CompressDbxBak
install -D -m 0755 $MBD/support-scripts/NfsMount.sh $RBR/%{_bindir}/NfsMount
install -D -m 0755 $MBD/support-scripts/RemoveDbxBak.sh $RBR/%{_bindir}/RemoveDbxBak
install -D -m 0755 $MBD/support-scripts/SyncDBxBak.sh $RBR/%{_bindir}/SyncDBxBak


%files
%{_bindir}/CompressDbxBak
%{_bindir}/DatabaseBackup
%{_bindir}/NfsMount
%{_bindir}/RemoveDbxBak
%{_bindir}/SyncDBxBak
%{_bindir}/read_metadata
%{_bindir}/registe_metadata
#%doc



%changelog

