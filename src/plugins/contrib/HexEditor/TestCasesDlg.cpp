#include "TestCasesDlg.h"

//(*InternalHeaders(TestCasesDlg)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(TestCasesDlg)
const long TestCasesDlg::ID_LISTBOX1 = wxNewId();
const long TestCasesDlg::ID_BUTTON1 = wxNewId();
const long TestCasesDlg::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TestCasesDlg,wxDialog)
	//(*EventTable(TestCasesDlg)
	//*)
END_EVENT_TABLE()

TestCasesDlg::TestCasesDlg(wxWindow* parent): m_Thread( 0 )
{
	BuildContent(parent);
}

void TestCasesDlg::BuildContent(wxWindow* parent)
{
	//(*Initialize(TestCasesDlg)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Test log:"));
	ListBox1 = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(410,268), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(ListBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	StaticBoxSizer1->Add(Button1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	Timer1.Start(50, false);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TestCasesDlg::OnButton1Click);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&TestCasesDlg::OnTimer1Trigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TestCasesDlg::OnClose);
	//*)

    m_Running     = true;
    m_StopRequest = false;
    m_BtnChanged  = false;

    // Start the thread
    m_Thread = new MyThread( this );
    m_Thread->Create();
    m_Thread->Run();
}

TestCasesDlg::~TestCasesDlg()
{
	//(*Destroy(TestCasesDlg)
	//*)
	m_Thread->Wait();
	delete m_Thread;
}

int TestCasesDlg::Entry()
{
    m_Result  = PerformTests();
    m_Running = false;
    return 0;
}

bool TestCasesDlg::StopTest()
{
    // Don't have to lock anything since
    // we only send one bit of information
    return m_StopRequest;
}

void TestCasesDlg::AddLog( const wxString& logLine )
{
    wxCriticalSectionLocker lock( m_Section );
    m_NewLogs.Add( logLine );
}

void TestCasesDlg::OnButton1Click(wxCommandEvent& event)
{
    if ( m_Running )
    {
        m_StopRequest = true;
        Button1->Disable();
        AddLog( _T("Cancelled by the user") );
    }
    else if ( m_BtnChanged )
    {
        EndDialog( wxID_OK );
    }
}

void TestCasesDlg::OnTimer1Trigger(wxTimerEvent& event)
{
    if ( !m_Running && !m_BtnChanged )
    {
        m_BtnChanged = true;
        Button1->Enable();
        Button1->SetLabel( _("Close") );
    }

    wxCriticalSectionLocker lock( m_Section );
    if ( !m_NewLogs.IsEmpty() )
    {
        ListBox1->Append( m_NewLogs );
        m_NewLogs.clear();

        ListBox1->SetFirstItem( ListBox1->GetCount() - 1 );
    }
}

void TestCasesDlg::OnClose(wxCloseEvent& event)
{
    if ( m_Running && event.CanVeto() )
    {
        event.Veto();
    }
    else
    {
        m_StopRequest = true;
        event.Skip();
    }
}
