#if defined(_WIN32) || defined(linux)
// this compiler is valid only in windows and linux

#include <sdk.h>
#include "compilerDMD.h"
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerDMD::CompilerDMD()
    : Compiler(_("Digital Mars D Compiler"), _T("dmd"))
{
    Reset();
}

CompilerDMD::~CompilerDMD()
{
	//dtor
}

Compiler * CompilerDMD::CreateCopy()
{
    Compiler* c = new CompilerDMD(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerDMD::Reset()
{
	#ifdef __WXMSW__
	m_Programs.C = _T("dmd.exe");
	m_Programs.CPP = _T("dmd.exe");
	m_Programs.LD = _T("dmd.exe");
	m_Programs.LIB = _T("lib.exe");
	m_Programs.DBG = _T("windbg.exe");
	m_Programs.WINDRES = _T("dmd.exe");
	m_Programs.MAKE = _T("make.exe");

	m_Switches.includeDirs = _T("-I");
	m_Switches.libDirs = _T("");
	m_Switches.linkLibs = _T("");
	m_Switches.libPrefix = _T("");
	m_Switches.libExtension = _T("lib");
	m_Switches.defines = _T("");
	m_Switches.genericSwitch = _T("-");
	m_Switches.objectExtension = _T("obj");
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

	// FIXME (hd#1#): should be work on: we need $res_options
	m_Commands[(int)ctCompileResourceCmd] = _T("$rescomp $resource_output $res_includes $file");
	m_Commands[(int)ctLinkExeCmd] = _T("$linker $exe_output $link_options $link_objects $libs");
	m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker $exe_output $link_options $link_objects $libs");

	#else // linux
	m_Programs.C = _T("dmd");
	m_Programs.CPP = _T("dmd");
	m_Programs.LD = _T("gcc");
	m_Programs.LIB = _T("ar");
	m_Programs.DBG = _T("gdb");
	m_Programs.WINDRES = _T("");
	m_Programs.MAKE = _T("make");

	m_Switches.includeDirs = _T("-I");
	m_Switches.libDirs = _T("-L");
	m_Switches.linkLibs = _T("-l");
	m_Switches.libPrefix = _T("lib");
	m_Switches.libExtension = _T("a");
	m_Switches.defines = _T("");
	m_Switches.genericSwitch = _T("-");
	m_Switches.objectExtension = _T("o");
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = false;

 	m_Commands[(int)ctCompileResourceCmd] = _T("");
	m_Commands[(int)ctLinkExeCmd] = _T("$linker -o $exe_output $link_options $link_objects $libs");
	m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker -o $exe_output $link_options $link_objects $libs");
	#endif

    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler $options $includes -c $file -of$object");
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker $exe_output $link_options $link_objects $libs $link_resobjects");
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker $static_output $link_options $link_objects");
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    m_Options.ClearOptions();

	//. m_Options.AddOption(_("Alignment of struct members"), "-a[1|2|4|8]", _("Architecture"));
	//m_Options.AddOption(_("compile only, do not link"), _T("-c"), _("D Features"));
    m_Options.AddOption(_("instrument for code coverage analysis"), _T("-cov"), _("D Features"));
    m_Options.AddOption(_("generate documentation"), _T("-D"), _("D Features"));
    m_Options.AddOption(_("allow deprecated features"), _T("-d"), _("D Features"));
    m_Options.AddOption(_("compile in debug code"), _T("-debug"), _("D Features"));
    m_Options.AddOption(_("add symbolic debug info"), _T("-g"), _("D Features"));
    m_Options.AddOption(_("generate D interface file"), _T("-H"), _("Others"));
    m_Options.AddOption(_("inline expand functions"), _T("-inline"), _("Optimize"));
    m_Options.AddOption(_("optimize"), _T("-O"), _("D Features"));
    m_Options.AddOption(_("suppress generation of object file"), _T("-o-"), _("D Features"));
    m_Options.AddOption(_("profile the runtime performance of the generated code"), _T("-profile"), _("Debugging"));
    m_Options.AddOption(_("suppress non-essential compiler messages"), _T("-quiet"), _("Others"));
    m_Options.AddOption(_("compile release version, which means not generating code for contracts and asserts"), _T("-release"), _("D Features"));
    m_Options.AddOption(_("compile in unittest code, also turns on asserts"), _T("-unittest"), _("Debugging"));
    m_Options.AddOption(_("verbose"), _T("-v"), _("Others"));
    m_Options.AddOption(_("enable warnings"), _T("-w"), _("Others"));

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
	#ifndef __WXMSW__
	m_LinkLibs.Add(_("pthread"));
	m_LinkLibs.Add(_("m"));
	#endif
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerDMD::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltError, _T("warning - ([ \\tA-Za-z0-9_:+/\\.-]+)\\(([0-9]+)\\):[ \\t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("([ \\tA-Za-z0-9_:+/\\.-]+)\\(([0-9]+)\\):[ \\t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("Error ([0-9]+):[\\s]*(.*)"), 2));
    m_RegExes.Add(RegExStruct(_("Linker warning"), cltError, _T("Error ([0-9]+):[\\s]*(.*)"), 2));
}

AutoDetectResult CompilerDMD::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();

    // NOTE (hd#1#): dmc uses sc.ini for compiler's master directories
    // NOTE (mandrav#1#): which doesn't seem to exist if you don't have the CD version ;)

    // just a guess; the default installation dir
	#ifdef __WXMSW__
	m_MasterPath = _T("C:\\dmd");
	wxString incPath=m_MasterPath + sep + _T("src") + sep + _T("phobos");
	wxString libPath=m_MasterPath + sep + _T("lib");
	wxString libName=_T("phobos.lib");
	#else // linux
	m_MasterPath = wxFileExists(_T("/usr/local/bin/dmd")) ? _T("/usr/local") : _T("/usr");
	wxString incPath=m_MasterPath + sep + _T("lib") + sep + _T("phobos");
	wxString libPath=m_MasterPath + sep + _T("lib");
	wxString libName=_T("phobos");
	#endif

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(incPath);
        AddLibDir(libPath);
    }
    AddLinkLib(libName);

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

#endif // _WIN32 || linux
