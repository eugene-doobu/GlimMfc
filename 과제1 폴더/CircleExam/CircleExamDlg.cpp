//
// CircleExamDlg.cpp: implementation file
//

#include "pch.h"
#include "framework.h"
#include "CircleExam.h"
#include "CircleExamDlg.h"
#include "DlgImage.h"
#include "afxdialogex.h"

#include <ctime>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void threadRandomMove(HWND hWnd)
{
	if (hWnd == NULL)
	{
		return;
	}

	for (int i = 0; i < 10; ++i)
	{
		if (!::IsWindow(hWnd))
		{
			return;
		}

		::PostMessage(hWnd, WM_COMMAND, ID_RANDOM_MOVE_STEP, 0);
		::Sleep(500);
	}

	if (::IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_COMMAND, ID_RANDOM_MOVE_DONE, 0);
	}
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CCircleExamDlg::CCircleExamDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLEEXAM_DIALOG, pParent)
	, m_pImgDlg(nullptr)
	, m_bRandomMoving(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCircleExamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCircleExamDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_RESET, &CCircleExamDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CCircleExamDlg::OnBnClickedBtnRandom)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &CCircleExamDlg::OnEnChangeDrawingInput)
	ON_EN_CHANGE(IDC_EDIT_THICKNESS, &CCircleExamDlg::OnEnChangeDrawingInput)
	ON_COMMAND(ID_RANDOM_MOVE_STEP, &CCircleExamDlg::OnRandomMoveStep)
	ON_COMMAND(ID_RANDOM_MOVE_DONE, &CCircleExamDlg::OnRandomMoveDone)
END_MESSAGE_MAP()

BOOL CCircleExamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid = FALSE;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	MoveWindow(80, 60, 1500, 910);
	SetWindowText(_T("CircleExam"));
	srand((unsigned int)time(NULL));

	SetDlgItemInt(IDC_EDIT_RADIUS, DEFAULT_POINT_RADIUS, FALSE);
	SetDlgItemInt(IDC_EDIT_THICKNESS, DEFAULT_CIRCLE_THICKNESS, FALSE);

	m_pImgDlg = new CDlgImage(this);
	if (m_pImgDlg != nullptr)
	{
		m_pImgDlg->Create(IDD_DLGIMAGE, this);
		m_pImgDlg->ShowWindow(SW_SHOW);
		m_pImgDlg->MoveWindow(20, 20, 1200, 800);
	}

	UpdatePointLabels(nullptr, 0);

	return TRUE;
}

void CCircleExamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CCircleExamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CCircleExamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCircleExamDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pImgDlg != nullptr)
	{
		if (::IsWindow(m_pImgDlg->GetSafeHwnd()))
		{
			m_pImgDlg->DestroyWindow();
		}
		delete m_pImgDlg;
		m_pImgDlg = nullptr;
	}
}

void CCircleExamDlg::OnBnClickedBtnReset()
{
	if (m_pImgDlg != nullptr)
	{
		m_pImgDlg->ResetState();
	}
}

void CCircleExamDlg::OnBnClickedBtnRandom()
{
	if (m_bRandomMoving || m_pImgDlg == nullptr || !m_pImgDlg->HasThreePoints())
	{
		return;
	}

	m_bRandomMoving = TRUE;
	SetActionButtonsEnabled(FALSE);

	std::thread _thread0(threadRandomMove, GetSafeHwnd());
	_thread0.detach();
}

void CCircleExamDlg::OnEnChangeDrawingInput()
{
	if (m_pImgDlg != nullptr)
	{
		m_pImgDlg->Redraw();
	}
}

void CCircleExamDlg::OnRandomMoveStep()
{
	if (m_pImgDlg != nullptr)
	{
		m_pImgDlg->RandomizePoints();
	}
}

void CCircleExamDlg::OnRandomMoveDone()
{
	m_bRandomMoving = FALSE;
	SetActionButtonsEnabled(TRUE);
}

int CCircleExamDlg::GetPointRadius() const
{
	return GetPositiveEditInt(IDC_EDIT_RADIUS, DEFAULT_POINT_RADIUS);
}

int CCircleExamDlg::GetCircleThickness() const
{
	return GetPositiveEditInt(IDC_EDIT_THICKNESS, DEFAULT_CIRCLE_THICKNESS);
}

void CCircleExamDlg::UpdatePointLabels(const CPoint* pts, int count)
{
	SetPointLabel(IDC_STATIC_P1, _T("P1"), count > 0 ? &pts[0] : nullptr);
	SetPointLabel(IDC_STATIC_P2, _T("P2"), count > 1 ? &pts[1] : nullptr);
	SetPointLabel(IDC_STATIC_P3, _T("P3"), count > 2 ? &pts[2] : nullptr);
}

void CCircleExamDlg::SetPointLabel(int controlId, LPCTSTR prefix, const CPoint* pt)
{
	CString text;
	if (pt != nullptr)
	{
		text.Format(_T("%s: (%d, %d)"), prefix, pt->x, pt->y);
	}
	else
	{
		text.Format(_T("%s: (-, -)"), prefix);
	}

	SetDlgItemText(controlId, text);
}

int CCircleExamDlg::GetPositiveEditInt(int controlId, int defaultValue) const
{
	BOOL translated = FALSE;
	const UINT value = GetDlgItemInt(controlId, &translated, FALSE);
	if (!translated || value == 0)
	{
		return defaultValue;
	}

	return static_cast<int>(value);
}

void CCircleExamDlg::SetActionButtonsEnabled(BOOL bEnable)
{
	CWnd* pReset = GetDlgItem(IDC_BTN_RESET);
	CWnd* pRandom = GetDlgItem(IDC_BTN_RANDOM);

	if (pReset != nullptr)
	{
		pReset->EnableWindow(bEnable);
	}

	if (pRandom != nullptr)
	{
		pRandom->EnableWindow(bEnable);
	}
}
