Name:       ParateraDB-Utils	
Version:    1.0.11
Release:	1%{?dist}
Summary:	ParateraDB Utils Tools.

Group:      Databases/ParateraDB	
License:    Copyright(c) 2015	
URL:	    https://git.paratera.net/dba/ParateraDB-Utils	
Source0:    https://git.paratera.net/dba/%{name}.tar.gz	


BuildRequires: coreutils grep procps shadow-utils gcc-c++ gperf ncurses-devel perl readline-devel time zlib-devel libaio-devel bison cmake make automake	

AutoReq: no

%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(AdvisorRules\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(Diskstats.*\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(Lmo.*\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(NibbleIterator\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(Percona::Toolkit\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(Quoter\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(ReadKeyMini\\)$
%global __requires_exclude %{?__requires_exclude:%__requires_exclude|}^perl\\(Transformers\\)$

Requires:       mailx tar openssl grep coreutils procps shadow-utils perl time pigz ParateraDB-Backup ParateraDB-Server-server-56 ParateraDB-Server-client-56	
Requires:       perl(:MODULE_COMPAT_%(eval "`%{__perl} -V:version`"; echo $version))
Requires:       perl(DBD::mysql)

%description
    ParateraDB Utils Tools.

%prep
%setup -q -n %{name}


%build
make %{?_smp_mflags}


%install

RBR=$RPM_BUILD_ROOT
MBD=$RPM_BUILD_DIR/%{name}

install -D -m 0755 $MBD/read_metadata $RBR/%{_bindir}/read_metadata
install -D -m 0755 $MBD/pdb $RBR/%{_bindir}/pdb
install -D -m 0755 $MBD/registe_metadata $RBR/%{_bindir}/registe_metadata
install -D -m 0755 $MBD/support-scripts/DatabaseBackup.sh $RBR/%{_bindir}/DatabaseBackup
install -D -m 0755 $MBD/support-scripts/CompressDbxBak.sh $RBR/%{_bindir}/CompressDbxBak
install -D -m 0755 $MBD/support-scripts/NfsMount.sh $RBR/%{_bindir}/NfsMount
install -D -m 0755 $MBD/support-scripts/RemoveDbxBak.sh $RBR/%{_bindir}/RemoveDbxBak
install -D -m 0755 $MBD/support-scripts/SyncDBxBak.sh $RBR/%{_bindir}/SyncDBxBak
install -D -m 0755 $MBD/support-scripts/pt-align $RBR/%{_bindir}/pt-align
install -D -m 0755 $MBD/support-scripts/pt-archiver $RBR/%{_bindir}/pt-archiver
install -D -m 0755 $MBD/support-scripts/pt-config-diff $RBR/%{_bindir}/pt-config-diff
install -D -m 0755 $MBD/support-scripts/pt-deadlock-logger $RBR/%{_bindir}/pt-deadlock
install -D -m 0755 $MBD/support-scripts/pt-diskstats $RBR/%{_bindir}/pt-diskstats
install -D -m 0755 $MBD/support-scripts/pt-duplicate-key-checker $RBR/%{_bindir}/pt-duplicate-key-checker
install -D -m 0755 $MBD/support-scripts/pt-fifo-split $RBR/%{_bindir}/pt-fifo-split
install -D -m 0755 $MBD/support-scripts/pt-find $RBR/%{_bindir}/pt-find
install -D -m 0755 $MBD/support-scripts/pt-fingerprint $RBR/%{_bindir}/pt-fingerprint
install -D -m 0755 $MBD/support-scripts/pt-fk-error-logger $RBR/%{_bindir}/pt-fk-error-logger
install -D -m 0755 $MBD/support-scripts/pt-heartbeat $RBR/%{_bindir}/pt-heartbeatd
install -D -m 0755 $MBD/support-scripts/pt-index-usage $RBR/%{_bindir}/pt-index-usage
install -D -m 0755 $MBD/support-scripts/pt-ioprofile $RBR/%{_bindir}/pt-ioprofile
install -D -m 0755 $MBD/support-scripts/pt-kill $RBR/%{_bindir}/pt-kill
install -D -m 0755 $MBD/support-scripts/pt-mext $RBR/%{_bindir}/pt-mext
install -D -m 0755 $MBD/support-scripts/pt-mysql-summary $RBR/%{_bindir}/pt-mysql-summary
install -D -m 0755 $MBD/support-scripts/pt-online-schema-change $RBR/%{_bindir}/pt-online-schema-change
install -D -m 0755 $MBD/support-scripts/pt-pmp $RBR/%{_bindir}/pt-pmp
install -D -m 0755 $MBD/support-scripts/pt-query-digest $RBR/%{_bindir}/pt-query-digest
install -D -m 0755 $MBD/support-scripts/pt-show-grants $RBR/%{_bindir}/pt-show-grants
install -D -m 0755 $MBD/support-scripts/pt-sift $RBR/%{_bindir}/pt-sift
install -D -m 0755 $MBD/support-scripts/pt-slave-delay $RBR/%{_bindir}/pt-slave-delay
install -D -m 0755 $MBD/support-scripts/pt-slave-find $RBR/%{_bindir}/pt-slave-find
install -D -m 0755 $MBD/support-scripts/pt-slave-restart $RBR/%{_bindir}/pt-slave-restart
install -D -m 0755 $MBD/support-scripts/pt-stalk $RBR/%{_bindir}/pt-stalk
install -D -m 0755 $MBD/support-scripts/pt-summary $RBR/%{_bindir}/pt-summary
install -D -m 0755 $MBD/support-scripts/pt-table-checksum $RBR/%{_bindir}/pt-table-checksum
install -D -m 0755 $MBD/support-scripts/pt-table-sync $RBR/%{_bindir}/pt-table-sync
install -D -m 0755 $MBD/support-scripts/pt-table-usage $RBR/%{_bindir}/pt-table-usage
install -D -m 0755 $MBD/support-scripts/pt-upgrade $RBR/%{_bindir}/pt-upgrade
install -D -m 0755 $MBD/support-scripts/pt-variable-advisor $RBR/%{_bindir}/pt-variable-advisor
install -D -m 0755 $MBD/support-scripts/pt-visual-explain $RBR/%{_bindir}/pt-visual-explain
install -D -m 0755 $MBD/support-scripts/post.sh $RBR/%{_bindir}/post.sh
cp -r $MBD/doc/doc $RBR/%{_datadir}/
cp -r $MBD/doc/man $RBR/%{_datadir}/

%post 
%{_bindir}/post.sh

%files
%{_bindir}/post.sh
%{_bindir}/CompressDbxBak
%{_bindir}/DatabaseBackup
%{_bindir}/NfsMount
%{_bindir}/RemoveDbxBak
%{_bindir}/SyncDBxBak
%{_bindir}/read_metadata
%{_bindir}/pdb
%{_bindir}/registe_metadata
%{_bindir}/pt-align
%{_bindir}/pt-archiver
%{_bindir}/pt-config-diff
%{_bindir}/pt-deadlock
%{_bindir}/pt-diskstats
%{_bindir}/pt-duplicate-key-checker
%{_bindir}/pt-fifo-split
%{_bindir}/pt-find
%{_bindir}/pt-fingerprint
%{_bindir}/pt-fk-error-logger
%{_bindir}/pt-heartbeatd
%{_bindir}/pt-index-usage
%{_bindir}/pt-ioprofile
%{_bindir}/pt-kill
%{_bindir}/pt-mext
%{_bindir}/pt-mysql-summary
%{_bindir}/pt-online-schema-change
%{_bindir}/pt-pmp
%{_bindir}/pt-query-digest
%{_bindir}/pt-show-grants
%{_bindir}/pt-sift
%{_bindir}/pt-slave-delay
%{_bindir}/pt-slave-find
%{_bindir}/pt-slave-restart
%{_bindir}/pt-stalk
%{_bindir}/pt-summary
%{_bindir}/pt-table-checksum
%{_bindir}/pt-table-sync
%{_bindir}/pt-table-usage
%{_bindir}/pt-upgrade
%{_bindir}/pt-variable-advisor
%{_bindir}/pt-visual-explain
%{_datadir}/man/man1/percona-toolkit.1p.gz
%{_datadir}/man/man1/pt-align.1p.gz
%{_datadir}/man/man1/pt-archiver.1p.gz
%{_datadir}/man/man1/pt-config-diff.1p.gz
%{_datadir}/man/man1/pt-deadlock-logger.1p.gz
%{_datadir}/man/man1/pt-diskstats.1p.gz
%{_datadir}/man/man1/pt-duplicate-key-checker.1p.gz
%{_datadir}/man/man1/pt-fifo-split.1p.gz
%{_datadir}/man/man1/pt-find.1p.gz
%{_datadir}/man/man1/pt-fingerprint.1p.gz
%{_datadir}/man/man1/pt-fk-error-logger.1p.gz
%{_datadir}/man/man1/pt-heartbeat.1p.gz
%{_datadir}/man/man1/pt-index-usage.1p.gz
%{_datadir}/man/man1/pt-ioprofile.1p.gz
%{_datadir}/man/man1/pt-kill.1p.gz
%{_datadir}/man/man1/pt-mext.1p.gz
%{_datadir}/man/man1/pt-mysql-summary.1p.gz
%{_datadir}/man/man1/pt-online-schema-change.1p.gz
%{_datadir}/man/man1/pt-pmp.1p.gz
%{_datadir}/man/man1/pt-query-digest.1p.gz
%{_datadir}/man/man1/pt-show-grants.1p.gz
%{_datadir}/man/man1/pt-sift.1p.gz
%{_datadir}/man/man1/pt-slave-delay.1p.gz
%{_datadir}/man/man1/pt-slave-find.1p.gz
%{_datadir}/man/man1/pt-slave-restart.1p.gz
%{_datadir}/man/man1/pt-stalk.1p.gz
%{_datadir}/man/man1/pt-summary.1p.gz
%{_datadir}/man/man1/pt-table-checksum.1p.gz
%{_datadir}/man/man1/pt-table-sync.1p.gz
%{_datadir}/man/man1/pt-table-usage.1p.gz
%{_datadir}/man/man1/pt-upgrade.1p.gz
%{_datadir}/man/man1/pt-variable-advisor.1p.gz
%{_datadir}/man/man1/pt-visual-explain.1p.gz
%{_datadir}/percona-toolkit-2.2.15/COPYING
%{_datadir}/percona-toolkit-2.2.15/Changelog
%{_datadir}/percona-toolkit-2.2.15/INSTALL
%{_datadir}/percona-toolkit-2.2.15/README
#%doc

%changelog

