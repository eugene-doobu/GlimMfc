//
// CircleExamDlg.h: header file
//

#pragma once

class CDlgImage;

#define DEFAULT_POINT_RADIUS 12
#define DEFAULT_CIRCLE_THICKNESS 4

class CCircleExamDlg : public CDialogEx
{
public:
	CCircleExamDlg(CWnd* pParent = nullptr);

	int GetPointRadius() const;
	int GetCircleThickness() const;
	void UpdatePointLabels(const CPoint* pts, int count);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLEEXAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;
	CDlgImage* m_pImgDlg;
	BOOL m_bRandomMoving;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnRandom();
	afx_msg void OnEnChangeDrawingInput();
	afx_msg void OnRandomMoveStep();
	afx_msg void OnRandomMoveDone();
	DECLARE_MESSAGE_MAP()

private:
	static UINT RandomMoveThread(LPVOID pParam);

	void SetPointLabel(int controlId, LPCTSTR prefix, const CPoint* pt);
	int GetPositiveEditInt(int controlId, int defaultValue) const;
	void SetActionButtonsEnabled(BOOL bEnable);
};
