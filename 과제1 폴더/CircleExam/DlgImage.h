//
// DlgImage.h: header file
//

#pragma once

#include <atlimage.h>

class CDlgImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImage)

public:
	CDlgImage(CWnd* pParent = nullptr);
	virtual ~CDlgImage();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGIMAGE };
#endif

	void ResetState();
	void Redraw();
	void RandomizePoints();
	bool HasThreePoints() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	enum
	{
		IMAGE_WIDTH = 1200,
		IMAGE_HEIGHT = 800
	};

	void InitImage();
	void ClearImage();
	void UpdateDisplay();
	void UpdateCoordinateDisplay();
	bool HitTestPoint(const CPoint& point, int& index) const;
	int GetCurrentRadius() const;
	int GetCurrentThickness() const;

	static int Clamp(int value, int minValue, int maxValue);
	static bool IsInCircle(int x, int y, int cx, int cy, int radius);
	static bool CalcCircumscribedCircle(
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double& centerX, double& centerY, double& radius);
	static void DrawFilledCircle(
		unsigned char* bits,
		int pitch,
		int width,
		int height,
		int centerX,
		int centerY,
		int radius,
		unsigned char color);
	static void DrawCircleOutline(
		unsigned char* bits,
		int pitch,
		int width,
		int height,
		double centerX,
		double centerY,
		double radius,
		int thickness,
		unsigned char color);

	CWnd* m_pParentWnd;
	CImage m_image;
	CPoint m_pts[3];
	int m_nClickCount;
	int m_nDragIdx;
	bool m_bDragging;
};
