Name:       ParateraDB-Utils	
Version:    1 
Release:	1%{?dist}
Summary:	ParateraDB Utils Tools.

Group:      Databases/ParateraDB	
License:    Copyright(c) 2015	
URL:	    https://git.paratera.net/dba/ParateraDB-Utils	
Source0:    https://git.paratera.net/dba/ParateraDB-Utils.tar.gz	

#BuildRequires:	
#Requires:	

%description
    ParateraDB Utils Tools.

%prep
%setup -q


%build
make %{?_smp_mflags}


%install

RBR=$RPM_BUILD_ROOT
MBD=$RPM_BUILD_DIR/%{name}-%{version}

install -D -m 0755 $MBD/read_metadata $RBR/opt/pdb/bin/read_metadata
install -D -m 0755 $MBD/registe_metadata $RBR/opt/pdb/bin/registe_metadata
install -D -m 0755 $MBD/support-scripts/DatabaseBackup.sh $RBR/opt/pdb/bin/DatabaseBackup
install -D -m 0755 $MBD/support-scripts/CompressDbxBak.sh $RBR/opt/pdb/bin/CompressDbxBak
install -D -m 0755 $MBD/support-scripts/NfsMount.sh $RBR/opt/pdb/bin/NfsMount
install -D -m 0755 $MBD/support-scripts/RemoveDbxBak.sh $RBR/opt/pdb/bin/RemoveDbxBak
install -D -m 0755 $MBD/support-scripts/SyncDBxBak.sh $RBR/opt/pdb/bin/SyncDBxBak


%files
/opt/pdb/bin/CompressDbxBak
/opt/pdb/bin/DatabaseBackup
/opt/pdb/bin/NfsMount
/opt/pdb/bin/RemoveDbxBak
/opt/pdb/bin/SyncDBxBak
/opt/pdb/bin/read_metadata
/opt/pdb/bin/registe_metadata
#%doc



%changelog

