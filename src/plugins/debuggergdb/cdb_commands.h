#ifndef CDB_DEBUGGER_COMMANDS_H
#define CDB_DEBUGGER_COMMANDS_H

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <globals.h>
#include <manager.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "debuggertree.h"
#include "backtracedlg.h"

/**
  * Command to add a search directory for source files in debugger's paths.
  */
class CdbCmd_AddSourceDir : public DebuggerCmd
{
    public:
        /** If @c dir is empty, resets all search dirs to $cdir:$cwd, the default. */
        CdbCmd_AddSourceDir(DebuggerDriver* driver, const wxString& dir)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("directory ") << dir;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Warning: C:\Devel\tmp\console\111: No such file or directory.
            // Source directories searched: <dir>;$cdir;$cwd
            if (output.StartsWith(_T("Warning: ")))
                m_pDriver->Log(output.BeforeFirst(_T('\n')));
        }
};

/**
  * Command to the set the file to be debugged.
  */
class CdbCmd_SetDebuggee : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_SetDebuggee(DebuggerDriver* driver, const wxString& file)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("file ") << file;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Reading symbols from C:\Devel\tmp\console/console.exe...done.
            // or if it doesn't exist:
            // console.exe: No such file or directory.

            // just log everything before the prompt
            m_pDriver->Log(output.BeforeFirst(_T('\n')));
        }
};

/**
  * Command to the add symbol files.
  */
class CdbCmd_AddSymbolFile : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        CdbCmd_AddSymbolFile(DebuggerDriver* driver, const wxString& file)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("add-symbol-file ") << file;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            //
            // add symbol table from file "console.exe" at
            // Reading symbols from C:\Devel\tmp\console/console.exe...done.
            //
            // or if it doesn't exist:
            // add symbol table from file "console.exe" at
            // console.exe: No such file or directory.

            // just ignore the "add symbol" line and log the rest before the prompt
            m_pDriver->Log(output.AfterFirst(_T('\n')).BeforeLast(_T('\n')));
        }
};

/**
  * Command to set the arguments to the debuggee.
  */
class CdbCmd_SetArguments : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        CdbCmd_SetArguments(DebuggerDriver* driver, const wxString& args)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("set args ") << args;
        }
        void ParseOutput(const wxString& output)
        {
            // No output
        }
};

/**
  * Command to the attach to a process.
  */
class CdbCmd_AttachToProcess : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_AttachToProcess(DebuggerDriver* driver, int pid)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("attach ") << pid;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Attaching to process <pid>
            // or,
            // Can't attach to process.
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                if (lines[i].StartsWith(_T("Attaching")))
                    m_pDriver->Log(lines[i]);
                else if (lines[i].StartsWith(_T("Can't ")))
                {
                    // log this and quit debugging
                    m_pDriver->Log(lines[i]);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, _T("quit")));
                }
//                m_pDriver->DebugLog(lines[i]);
    		}
        }
};

/**
  * Command to the detach from the process.
  */
class CdbCmd_Detach : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_Detach(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T(".detach");
        }
        void ParseOutput(const wxString& output)
        {
            // output any return, usually "Detached"
            m_pDriver->Log(output);
        }
};

/**
  * Command to add a breakpoint.
  */
class CdbCmd_AddBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to set. */
        CdbCmd_AddBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (bp->enabled)
            {
                if (bp->func.IsEmpty())
                {
                    wxString out = m_BP->filename;
                    DebuggerGDB::ConvertToGDBFile(out);
                    QuoteStringIfNeeded(out);
                    // we add one to line,  because scintilla uses 0-based line numbers, while gdb uses 1-based
                    m_Cmd << _T("bu") << m_BP->bpNum << _T(' ');
                    if (m_BP->temporary)
                        m_Cmd << _T("/1 ");
                    m_Cmd << _T('`') << out << _T(":") << bp->line + 1 << _T('`');
                }
                //GDB workaround
                //Use function name if this is C++ constructor/destructor
                else
                {
                    if (!m_BP->temporary)
                        m_Cmd << _T("break ");
                    else
                        m_Cmd << _T("tbreak ");
                    m_Cmd << bp->func;
                }
                //end GDB workaround

                // condition and ignore count will be set in ParseOutput, where we 'll have the bp number
            }
        }
        void ParseOutput(const wxString& output)
        {
            // possible outputs (only output lines starting with ***):
            //
            // *** WARNING: Unable to verify checksum for Win32GUI.exe
            // *** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\USER32.dll -
            // *** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\GDI32.dll -
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].StartsWith(_T("*** ")))
                    m_pDriver->Log(lines[i]);
            }
        }

        DebuggerBreakpoint* m_BP;
};

/**
  * Command to remove a breakpoint.
  */
class CdbCmd_RemoveBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to remove. If NULL, all breakpoints are removed.
            @param deleteBPwhenDone If bp is not NULL and this is true the breakpoint will be deleted after removal. */
        CdbCmd_RemoveBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp, bool deleteBPwhenDone = false)
            : DebuggerCmd(driver),
            m_BP(bp),
            m_DeleteBPwhenDone(deleteBPwhenDone)
        {
            if (!bp)
                m_Cmd << _T("bc *");
            else
                m_Cmd << _T("bc ") << bp->bpNum;
        }
        void ParseOutput(const wxString& output)
        {
            // usually no output, so display whatever comes in
            if (!output.IsEmpty())
                m_pDriver->Log(output);
        }

        DebuggerBreakpoint* m_BP;
        bool m_DeleteBPwhenDone;
};

/**
  * Command to get info about local frame variables.
  */
class CdbCmd_InfoLocals : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
    public:
        /** @param tree The tree to display the locals. */
        CdbCmd_InfoLocals(DebuggerDriver* driver, DebuggerTree* dtree)
            : DebuggerCmd(driver),
            m_pDTree(dtree)
        {
            m_Cmd << _T("info locals");
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString locals;
    		locals << _T("Local variables = {");
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
                locals << lines[i] << _T(',');
            locals << _T("}") << _T('\n');
            m_pDTree->BuildTree(locals);
        }
};

/**
  * Command to get info about current function arguments.
  */
class CdbCmd_InfoArguments : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
    public:
        /** @param tree The tree to display the args. */
        CdbCmd_InfoArguments(DebuggerDriver* driver, DebuggerTree* dtree)
            : DebuggerCmd(driver),
            m_pDTree(dtree)
        {
            m_Cmd << _T("info args");
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString args;
    		args << _T("Function Arguments = {");
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
                args << lines[i] << _T(',');
            args << _T("}") << _T('\n');
            m_pDTree->BuildTree(args);
        }
};

/**
  * Command to get info about a watched variable.
  */
class CdbCmd_Watch : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        Watch* m_pWatch;
    public:
        /** @param tree The tree to display the watch. */
        CdbCmd_Watch(DebuggerDriver* driver, DebuggerTree* dtree, Watch* watch)
            : DebuggerCmd(driver),
            m_pDTree(dtree),
            m_pWatch(watch)
        {
            m_Cmd << _T("output ") << Watch::FormatCommand(m_pWatch->format) << _T(" ") << m_pWatch->keyword;
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString w;
    		w << m_pWatch->keyword << _T(" = ");
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
                w << lines[i] << _T(',');
            w << _T('\n');
            m_pDTree->BuildTree(w);
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class CdbCmd_TooltipEvaluation : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        wxTipWindow** m_pWin;
        wxRect m_WinRect;
        wxString m_What;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        CdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, wxTipWindow** win, const wxRect& tiprect)
            : DebuggerCmd(driver),
            m_pWin(win),
            m_WinRect(tiprect),
            m_What(what)
        {
            m_Cmd << _T("output ") << what;
        }
        void ParseOutput(const wxString& output)
        {
            wxString tip;
            if (output.StartsWith(_T("No symbol ")) || output.StartsWith(_T("Attempt to ")))
                tip = output;
            else
                tip = m_What + _T("=") + output;

            if (*m_pWin)
                (*m_pWin)->Destroy();
            *m_pWin = new wxTipWindow(Manager::Get()->GetAppWindow(), tip, 640, m_pWin, &m_WinRect);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a backtrace.
  */
class CdbCmd_Backtrace : public DebuggerCmd
{
        BacktraceDlg* m_pDlg;
    public:
        /** @param dlg The backtrace dialog. */
        CdbCmd_Backtrace(DebuggerDriver* driver, BacktraceDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("bt");
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            // #0  main () at main.cpp:8
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
    		    // #0  main (argc=1, argv=0x3e2440) at my main.cpp:15
//    		    wxRegEx re(_T("#([0-9]+)[ \t]+([A-Za-z0-9_:]+)[ \t]+\\([^)]*\\)[ \t]+at[ \t]+(.*):([0-9]+)"));
    		    // #0  0x004013cf in main () at main.cpp:11
    		    wxRegEx re(_T("#([0-9]+)[ \t]+(0x[A-Fa-f0-9]+)[ \t]+in[ \t]+([A-Za-z0-9_:]+)[ \t]+\\([^)]*\\)[ \t]+at[ \t]+(.*):([0-9]+)"));
    		    if (re.Matches(lines[i]))
    		    {
                    m_pDriver->DebugLog(_T("MATCH!"));
                    StackFrame sf;
                    sf.valid = true;
    		        re.GetMatch(lines[i], 1).ToLong(&sf.number);
    		        re.GetMatch(lines[i], 2).ToULong(&sf.address, 16);
    		        sf.function = re.GetMatch(lines[i], 3);
    		        sf.file = re.GetMatch(lines[i], 4);
    		        sf.line = re.GetMatch(lines[i], 5);
                    m_pDlg->AddFrame(sf);
    		    }
    		}
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to initialize a disassembly.
  */
class CdbCmd_DisassemblyInit : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_DisassemblyInit(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("frame");
        }
        void ParseOutput(const wxString& output)
        {
            // output is two lines describing the current frame:
            //
            // #0  main () at main.cpp:8
            // C:/Devel/tmp/console/main.cpp:8:63:beg:0x4013ba

            if (!m_pDlg)
                return;

            StackFrame sf;
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                if (!lines[i].StartsWith(g_EscapeChars)) // ->->
                {
                    // #0  main () at main.cpp:8
                    wxRegEx re(_T("#([0-9]+)[ \t]+([A-Za-z0-9_:]+) \\(\\) at"));
                    if (re.Matches(lines[i]))
                    {
                        re.GetMatch(lines[i], 1).ToLong(&sf.number);
                        sf.function = re.GetMatch(lines[i], 2);
                    }
                }
                else
                {
                    // C:/Devel/tmp/console/main.cpp:11:113:beg:0x4013cf
                    lines[i].Remove(0, 2); // remove ->->
                    wxRegEx reSource;
                    #ifdef __WXMSW__
                    reSource.Compile(_T("([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
                    #else
                    reSource.Compile(_T("([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
                    #endif
                    if ( reSource.Matches(lines[i]) )
                    {
                        sf.valid = true;
                        #ifdef __WXMSW__
                        sf.file = reSource.GetMatch(lines[i], 1) + reSource.GetMatch(lines[i], 2); // drive + path
                        sf.line = reSource.GetMatch(lines[i], 3);
                        reSource.GetMatch(lines[i], 4).ToULong(&sf.address, 16);
                        #else
                        sf.file = reSource.GetMatch(lines[i], 1);
                        sf.line = reSource.GetMatch(lines[i], 2);
                        reSource.GetMatch(lines[i], 3).ToULong(&sf.address, 16);
                        #endif
                        break; // we 're only interested for the top-level stack frame
                    }
                }
    		}
            m_pDlg->Clear(sf);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of CdbCmd_DisassemblyInit, which is chained-called.
  */
class CdbCmd_InfoRegisters : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_InfoRegisters(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("info registers");
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // eax            0x40e66666       1088841318
            // ecx            0x40cbf0 4246512
            // edx            0x77c61ae8       2009471720
            // ebx            0x4000   16384
            // esp            0x22ff50 0x22ff50
            // ebp            0x22ff78 0x22ff78
            // esi            0x22ef80 2289536
            // edi            0x5dd3f4 6149108
            // eip            0x4013c9 0x4013c9
            // eflags         0x247    583
            // cs             0x1b     27
            // ss             0x23     35
            // ds             0x23     35
            // es             0x23     35
            // fs             0x3b     59
            // gs             0x0      0

            if (!m_pDlg)
                return;

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                // eax            0x40e66666       1088841318
                wxRegEx re(_T("([A-Za-z0-9]+)[ \t]+(0x[0-9A-Za-z]+)"));
                if (re.Matches(lines[i]))
                {
                    long int addr;
                    re.GetMatch(lines[i], 2).ToLong(&addr, 16);
                    m_pDlg->SetRegisterValue(DisassemblyDlg::RegisterIndexFromName(re.GetMatch(lines[i], 1)), addr);
                }
    		}
//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of CdbCmd_DisassemblyInit, which is chained-called.
  */
class CdbCmd_Disassembly : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_Disassembly(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("disassemble");
            m_pDriver->QueueCommand(new CdbCmd_DisassemblyInit(driver, dlg)); // chain call
            m_pDriver->QueueCommand(new CdbCmd_InfoRegisters(driver, dlg)); // chain call
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // Dump of assembler code for function main:
            // 0x00401390 <main+0>:	push   ebp
            // ...
            // End of assembler dump.

            if (!m_pDlg)
                return;

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                // 0x00401390 <main+0>:	push   ebp
                wxRegEx re(_T("(0x[0-9A-Za-z]+)[ \t]+<.*>:[ \t]+(.*)"));
                if (re.Matches(lines[i]))
                {
                    long int addr;
                    re.GetMatch(lines[i], 1).ToLong(&addr, 16);
                    m_pDlg->AddAssemblerLine(addr, re.GetMatch(lines[i], 2));
                }
    		}
            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

#endif // DEBUGGER_COMMANDS_H
