Name:ParateraDB-Utils	
Version:0.1
Release:	1%{?dist}
Summary:	
    ParateraDB Utils tools.

Group: ParateraDB	
License:	
URL:		
Source0:	

BuildRequires:	
Requires:	

%description


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
%make_install


%files
%doc



%changelog

