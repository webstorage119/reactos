
/* $Id$
 *
 * PROJECT:     System regression tool for ReactOS
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        tools/sysreg/conf_parser.h
 * PURPOSE:     source symbol lookup
 * PROGRAMMERS: Johannes Anderwald (johannes.anderwald at sbox tugraz at)
 */


#include "sym_file.h"
#include "env_var.h"
#include "pipe_reader.h"
#include "conf_parser.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#define _FINDDATA_T_DEFINED
#include <io.h>
#include <time.h>


namespace System_
{

	using std::vector;
	string SymbolFile::VAR_ROS_OUTPUT = "ROS_OUTPUT";
	string SymbolFile::ROS_ADDR2LINE = "ROS_ADDR2LINE";
	string SymbolFile::m_SymbolPath= "";
	string SymbolFile::m_SymResolver= "";
	SymbolFile::SymbolMap SymbolFile::m_Map;

//---------------------------------------------------------------------------------------
	SymbolFile::SymbolFile()
	{

	}

//---------------------------------------------------------------------------------------
	SymbolFile::~SymbolFile()
	{

	}

//---------------------------------------------------------------------------------------
	bool SymbolFile::initialize(ConfigParser & conf_parser, string const &Path) 
	{
		vector<string> vect;
		string current_dir;

		if (Path == "")
		{
			current_dir = "output-i386";
			EnvironmentVariable::getValue(SymbolFile::VAR_ROS_OUTPUT, current_dir);
		}
		else
		{
			current_dir = Path;
		}

		m_SymbolPath = current_dir;

		if (!conf_parser.getStringValue (ROS_ADDR2LINE, m_SymResolver))
		{
			cerr << "Warning: ROS_ADDR2LINE is not set in configuration file -> symbol lookup will fail" <<endl;
			return false;
		}

		string val = current_dir;
		val.insert (val.length()-1, "\\*");

		struct _finddatai64_t c_file;
		intptr_t hFile = _findfirsti64(val.c_str(), &c_file);

		if (hFile == -1L)
		{
			cerr << "SymbolFile::initialize> failed" <<endl;
			return false;
		}

		do
		{

			do
			{
				char * pos;
				if ((pos = strstr(c_file.name, ".nostrip.")))
				{
					size_t len = strlen(pos);
					string modulename = c_file.name;
					string filename = modulename;
					modulename.erase(modulename.length() - len, len);

					string path = current_dir;
					path.insert (path.length () -1, "\\");
					path.insert (path.length () -1, filename);
#ifdef NDEBUG				
					cerr << "Module Name " << modulename << endl << "File Name " << path << endl;
#endif

					m_Map.insert(std::make_pair<string, string>(modulename, path));

				}
				if (c_file.attrib & _A_SUBDIR)
				{
					if (c_file.name[0] != _T('.'))
					{
						string path = current_dir;
						path.insert (path.length ()-1, "\\");
						path.insert (path.length ()-1, c_file.name);
						vect.push_back (path);
					}
				}

			}while(_tfindnexti64(hFile, &c_file) == 0);

			_findclose(hFile);
			hFile = -1L;

			while(!vect.empty ())
			{
				current_dir = vect.front ();
				vect.erase (vect.begin());
				val = current_dir;
				val.insert (val.length() -1, "\\*");
				hFile = _tfindfirsti64(val.c_str(), &c_file);
				if (hFile != -1L)
				{
					break;
				}
			}

			if (hFile == -1L)
			{
				break;
			}

		}while(1);


		return !m_Map.empty();
	}

//---------------------------------------------------------------------------------------
	bool SymbolFile::resolveAddress(const string &module_name, const string &module_address, string &Buffer) 
	{
		SymbolMap::const_iterator it = m_Map.find (module_name);

		if (it == m_Map.end () || m_SymResolver == "")
		{
			cerr << "SymbolFile::resolveAddress> no symbol file or ROS_ADDR2LINE not set" << endl;
			return false;
		}

		char szCmd[300];

		sprintf(szCmd, "%s %s %s", m_SymResolver.c_str (), it->second.c_str (), module_address.c_str());	
		string pipe_cmd(szCmd);
        vector<string> vect;        
		PipeReader pipe_reader;

		if (!pipe_reader.openSource(pipe_cmd))
		{
			cerr << "SymbolFile::resolveAddress> failed to open pipe" <<pipe_cmd <<endl;
			return false;
		}

		bool ret = pipe_reader.readSource (vect);
		pipe_reader.closeSource ();
		return ret;
	}

//---------------------------------------------------------------------------------------
	bool SymbolFile::getSymbolFilePath(string const &ModuleName, string &FilePath)
	{
		SymbolMap::const_iterator it = m_Map.find (ModuleName);

		if (it == m_Map.end ())
		{
			cerr << "SymbolFile::resolveAddress> no symbol file found for module " << ModuleName << endl;
			return false;
		}

		FilePath = it->second;
		return true;
	}

} // end of namespace System_
