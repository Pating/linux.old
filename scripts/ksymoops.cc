// ksymoops.cc v1.7 -- A simple filter to resolve symbols in Linux Oops-logs
// Copyright (C) 1995 Greg McGary <gkm@magilla.cichlid.com>
// compile like so: g++ -o ksymoops ksymoops.cc -liostream

//////////////////////////////////////////////////////////////////////////////

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to the
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// This is a simple filter to resolve EIP and call-trace symbols from
// a Linux kernel "Oops" log.  Supply the symbol-map file name as a
// command-line argument, and redirect the oops-log into stdin.  Out
// will come the EIP and call-trace in symbolic form.

//////////////////////////////////////////////////////////////////////////////

// BUGS:
// * Doesn't deal with line-prefixes prepended by syslog--strip
//   these off first, before submitting to ksymoops.
// * Only resolves operands of jump and call instructions.

#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

inline int strequ(char const* x, char const* y) { return (::strcmp(x, y) == 0); }
inline int strnequ(char const* x, char const* y, size_t n) { return (::strncmp(x, y, n) == 0); }

const int code_size = 20;

//////////////////////////////////////////////////////////////////////////////

class KSym
{
    friend class NameList;

  private:
    unsigned long address_;
    char* name_;
    long offset_;
    long extent_;

  private:
    istream& scan(istream&);
    ostream& print(ostream&) const;
    void set_extent(KSym const& next_ksym) { extent_ = next_ksym.address_ - address_; }

  public:
    friend istream& operator >> (istream& is, KSym& k) { return k.scan(is); }
    friend ostream& operator << (ostream& os, const KSym& k) { return k.print(os); }
};

istream&
KSym::scan(istream& is)
{
    is >> ::hex >> address_;
    char type;
    is >> type;
    char name[128];
    is >> name;
    name_ = new char [strlen(name)+1];
    strcpy(name_, name);
    offset_ = 0;
    return is;
}

ostream&
KSym::print(ostream& os) const
{
    os << ::hex << address_ + offset_ << ' ' << '<' << name_;
    if (offset_)
	os << '+' << ::hex << offset_ << '/' << ::hex << extent_;
    return os << '>';
}

//////////////////////////////////////////////////////////////////////////////

class NameList
{
  private:
    // Caution: Fixed Allocation!
    // This should suffice for awhile since 1.1.86 has only 2482 symbols.
    KSym ksyms_0_[8000];
    int cardinality_;

  public:
    NameList() : cardinality_(0) { }
    
  private:
    istream& scan(istream&);

  public:
    int valid() { return (cardinality_ > 0); }
	
    KSym* find(unsigned long address);
    void decode(unsigned char* code, long eip_addr);
    
  public:
    friend istream& operator >> (istream& is, NameList& n) { return n.scan(is); }
};

KSym*
NameList::find(unsigned long address)
{
    if (!valid())
	return 0;
    KSym* start = ksyms_0_;
    KSym* end = &ksyms_0_[cardinality_ - 1];
    if (address < start->address_ || address >= end->address_)
	return 0;

    KSym* mid;
    while (start <= end) {
	mid = &start[(end - start) / 2];
	if (mid->address_ < address)
	    start = mid + 1;
	else if (mid->address_ > address)
	    end = mid - 1;
	else
	    return mid;
    }
    while (mid->address_ > address)
	--mid;
    mid->offset_ = address - mid->address_;
    if (mid->offset_ > mid->extent_)
	clog << "Oops! " << *mid << endl;
    return mid;
}

void
NameList::decode(unsigned char* code, long eip_addr)
{
    /* This is a hack to avoid using gcc.  We create an object file by
       concatenating objfile_head, the twenty bytes of code, and
       objfile_tail.  */
    unsigned char objfile_head[] = {
	0x07, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char objfile_tail[] = {
	0x00, 0x90, 0x90, 0x90,
	0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x25, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00,
	'g',  'c',  'c',  '2',  '_',  'c',  'o',  'm',  
	'p',  'i',  'l',  'e',  'd',  '.',  '\0', '_',  
	'E',  'I',  'P',  '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0'
    };
    char const* objdump_command = "objdump -d oops_decode.o";
    char const* objfile_name = &objdump_command[11];
    ofstream objfile_stream(objfile_name);

    objfile_stream.write(objfile_head, sizeof(objfile_head));
    objfile_stream.write(code, code_size);
    objfile_stream.write(objfile_tail, sizeof(objfile_tail));
    objfile_stream.close();
    
    FILE* objdump_FILE = popen(objdump_command, "r");
    if (objdump_FILE == 0) {
	clog << "Sorry, without " << objdump_command << ", I can't disassemble the `Code' section." << endl;
	return;
    }
    
    char buf[1024];
    int lines = 0;
    while (fgets(buf, sizeof(buf), objdump_FILE)) {
	if (!strnequ(&buf[9], "<_EIP", 5))
	    continue;
	if (strstr(buf, " is out of bounds"))
	    break;
	lines++;
	cout << "Code: ";
	if (!valid()) {
	    cout << buf;
	    continue;
	}
	long offset = strtol(buf, 0, 16);
	char* bp_0 = strchr(buf, '>') + 2;
	KSym* ksym = find(eip_addr + offset);
	if (ksym)
	    cout << *ksym << ' ';
	char* bp = bp_0;
	while (!isspace(*bp))
	    bp++;
	while (isspace(*bp))
	    bp++;
	if (*bp != '0') {
	    cout << bp_0;
	} else if (*bp_0 == 'j' || strnequ(bp_0, "call", 4)) { // a jump or call insn
	    long rel_addr = strtol(bp, 0, 16);
	    ksym = find(eip_addr + rel_addr);
	    if (ksym) {
		*bp++ = '\0';
		cout << bp_0 << *ksym << endl;
	    } else
		cout << bp_0;
	} else {
	    cout << bp_0;
	}
    }
    if (!lines)
	clog << "Sorry, your " << objdump_command << " can't disassemble--you must upgrade your binutils." << endl;
    pclose(objdump_FILE);
    unlink(objfile_name);
}

istream&
NameList::scan(istream& is)
{
    KSym* ksyms = ksyms_0_;
    int cardinality = 0;
    while (!is.eof()) {
	is >> *ksyms;
	if (cardinality++ > 0)
	    ksyms[-1].set_extent(*ksyms);
	ksyms++;
    }
    cardinality_ = --cardinality;
    return is;
}

//////////////////////////////////////////////////////////////////////////////

char const* program_name;

void
usage()
{
    clog << "Usage: " << program_name << " [ System.map ] < oops-log" << endl;
    exit(1);
}

int
main(int argc, char** argv)
{
    char c;
    program_name = (argc--, *argv++);

    NameList names;
    if (argc > 1)
	usage();
    else if (argc == 1) {
	char const* map_file_name = (argc--, *argv++);
	ifstream map(map_file_name);
	if (map.bad())
	    clog << program_name << ": Can't open `" << map_file_name << "'" << endl;
	else {
	    map >> names;
	    cout << "Using `" << map_file_name << "' to map addresses to symbols." << endl;
	}
    }
    if (!names.valid())
	cout << "No symbol map.  I'll only show you disassembled code." << endl;
    cout << endl;

    char buffer[1024];
    while (!cin.eof())
    {
	long eip_addr;
	cin >> buffer;
	if (strequ(buffer, "EIP:") && names.valid()) {
	    cin >> ::hex >> eip_addr;
	    cin >> c >> c >> c;
	    cin >> ::hex >> eip_addr;
	    cin >> c >> c >> buffer;
	    if (!strequ(buffer, "EFLAGS:")) {
		clog << "Please strip the line-prefixes and rerun " << program_name << endl;
		exit(1);
	    }
	    KSym* ksym = names.find(eip_addr);
	    if (ksym)
		cout << ">>EIP: " << *ksym << endl;
	} else if (strequ(buffer, "Trace:") && names.valid()) {
	    unsigned long address;
	    while ((cin >> buffer) && 
		   (sscanf(buffer, " [<%x>]", &address) == 1) &&
		   address > 0xc) {
		cout << "Trace: ";
		KSym* ksym = names.find(address);
		if (ksym)
		    cout << *ksym;
		else
		    cout << ::hex << address;
		cout << endl;
	    }
	    cout << endl;
	}
	if (strequ(buffer, "ode:") || strequ(buffer, "Code:")) {
	    // The 'C' might have been consumed as a hex number
	    unsigned char code[code_size];
	    unsigned char* cp = code;
	    unsigned char* end = &code[code_size];
	    while (cp < end) {
		int c;
		cin >> ::hex >> c;
		*cp++ = c;
	    }
	    names.decode(code, eip_addr);
	}
    }
    cout << flush;

    return 0;
}
