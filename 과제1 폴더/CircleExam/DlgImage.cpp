//
// DlgImage.cpp: implementation file
//

#include "pch.h"
#include "framework.h"
#include "CircleExam.h"
#include "DlgImage.h"
#include "CircleExamDlg.h"
#include "afxdialogex.h"

#include <cstdlib>
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CDlgImage, CDialogEx)

CDlgImage::CDlgImage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLGIMAGE, pParent)
	, m_pParentWnd(pParent)
	, m_nClickCount(0)
	, m_nDragIdx(-1)
	, m_bDragging(false)
{
	for (int i = 0; i < 3; ++i)
	{
		m_pts[i] = CPoint(0, 0);
	}
}

CDlgImage::~CDlgImage()
{
	if (!m_image.IsNull())
	{
		m_image.Destroy();
	}
}

void CDlgImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgImage, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CDlgImage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitImage();
	UpdateCoordinateDisplay();

	return TRUE;
}

void CDlgImage::OnPaint()
{
	CPaintDC dc(this);
	if (!m_image.IsNull())
	{
		m_image.Draw(dc, 0, 0);
	}
}

void CDlgImage::OnLButtonDown(UINT nFlags, CPoint point)
{
	point.x = Clamp(point.x, 0, IMAGE_WIDTH - 1);
	point.y = Clamp(point.y, 0, IMAGE_HEIGHT - 1);

	if (m_nClickCount < 3)
	{
		m_pts[m_nClickCount] = point;
		++m_nClickCount;
		Redraw();
	}
	else
	{
		int hitIndex = -1;
		if (HitTestPoint(point, hitIndex))
		{
			m_nDragIdx = hitIndex;
			m_bDragging = true;
			SetCapture();
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CDlgImage::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging && m_nDragIdx >= 0)
	{
		point.x = Clamp(point.x, 0, IMAGE_WIDTH - 1);
		point.y = Clamp(point.y, 0, IMAGE_HEIGHT - 1);
		m_pts[m_nDragIdx] = point;
		Redraw();
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CDlgImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		m_nDragIdx = -1;
		ReleaseCapture();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CDlgImage::ResetState()
{
	m_nClickCount = 0;
	m_nDragIdx = -1;
	m_bDragging = false;

	for (int i = 0; i < 3; ++i)
	{
		m_pts[i] = CPoint(0, 0);
	}

	Redraw();
}

void CDlgImage::Redraw()
{
	if (m_image.IsNull())
	{
		InitImage();
	}

	ClearImage();

	unsigned char* bits = static_cast<unsigned char*>(m_image.GetBits());
	const int pitch = m_image.GetPitch();
	const int radius = GetCurrentRadius();
	const int thickness = GetCurrentThickness();

	for (int i = 0; i < m_nClickCount; ++i)
	{
		DrawFilledCircle(bits, pitch, IMAGE_WIDTH, IMAGE_HEIGHT, m_pts[i].x, m_pts[i].y, radius, 0x00);
	}

	if (m_nClickCount == 3)
	{
		double centerX = 0.0;
		double centerY = 0.0;
		double outerRadius = 0.0;
		if (CalcCircumscribedCircle(
			static_cast<double>(m_pts[0].x), static_cast<double>(m_pts[0].y),
			static_cast<double>(m_pts[1].x), static_cast<double>(m_pts[1].y),
			static_cast<double>(m_pts[2].x), static_cast<double>(m_pts[2].y),
			centerX, centerY, outerRadius))
		{
			DrawCircleOutline(bits, pitch, IMAGE_WIDTH, IMAGE_HEIGHT, centerX, centerY, outerRadius, thickness, 0x00);
		}
	}

	UpdateCoordinateDisplay();
	UpdateDisplay();
}

void CDlgImage::RandomizePoints()
{
	if (m_nClickCount < 3)
	{
		return;
	}

	int nRadius = GetCurrentRadius();
	for (int i = 0; i < 3; ++i)
	{
		int x = rand() % IMAGE_WIDTH;
		int y = rand() % IMAGE_HEIGHT;

		if (nRadius > 0)
		{
			x = Clamp(x, nRadius, IMAGE_WIDTH - nRadius - 1);
			y = Clamp(y, nRadius, IMAGE_HEIGHT - nRadius - 1);
		}

		m_pts[i] = CPoint(x, y);
	}

	m_nClickCount = 3;
	m_nDragIdx = -1;
	m_bDragging = false;
	Redraw();
}

bool CDlgImage::HasThreePoints() const
{
	return m_nClickCount == 3;
}

void CDlgImage::InitImage()
{
	if (!m_image.IsNull())
	{
		m_image.Destroy();
	}

	const int bpp = 8;
	m_image.Create(IMAGE_WIDTH, -IMAGE_HEIGHT, bpp);

	static RGBQUAD grayPalette[256];
	for (int i = 0; i < 256; ++i)
	{
		grayPalette[i].rgbRed = static_cast<BYTE>(i);
		grayPalette[i].rgbGreen = static_cast<BYTE>(i);
		grayPalette[i].rgbBlue = static_cast<BYTE>(i);
		grayPalette[i].rgbReserved = 0;
	}
	m_image.SetColorTable(0, 256, grayPalette);

	ClearImage();
}

void CDlgImage::ClearImage()
{
	if (m_image.IsNull())
	{
		return;
	}

	unsigned char* bits = static_cast<unsigned char*>(m_image.GetBits());
	const int pitch = m_image.GetPitch();
	memset(bits, 0xff, abs(pitch) * m_image.GetHeight());
}

void CDlgImage::UpdateDisplay()
{
	Invalidate(FALSE);
	UpdateWindow();
}

void CDlgImage::UpdateCoordinateDisplay()
{
	CCircleExamDlg* pParent = (CCircleExamDlg*)m_pParentWnd;
	if (pParent != nullptr)
	{
		pParent->UpdatePointLabels(m_pts, m_nClickCount);
	}
}

bool CDlgImage::HitTestPoint(const CPoint& point, int& index) const
{
	const int radius = GetCurrentRadius();

	for (int i = 0; i < m_nClickCount; ++i)
	{
		const int dx = point.x - m_pts[i].x;
		const int dy = point.y - m_pts[i].y;
		if ((dx * dx) + (dy * dy) <= (radius * radius))
		{
			index = i;
			return true;
		}
	}

	return false;
}

int CDlgImage::GetCurrentRadius() const
{
	CCircleExamDlg* pParent = (CCircleExamDlg*)m_pParentWnd;
	if (pParent != nullptr)
	{
		return pParent->GetPointRadius();
	}

	return DEFAULT_POINT_RADIUS;
}

int CDlgImage::GetCurrentThickness() const
{
	CCircleExamDlg* pParent = (CCircleExamDlg*)m_pParentWnd;
	if (pParent != nullptr)
	{
		return pParent->GetCircleThickness();
	}

	return DEFAULT_CIRCLE_THICKNESS;
}

int CDlgImage::Clamp(int value, int minValue, int maxValue)
{
	if (value < minValue)
	{
		return minValue;
	}

	if (value > maxValue)
	{
		return maxValue;
	}

	return value;
}

bool CDlgImage::IsInCircle(int x, int y, int cx, int cy, int radius)
{
	const int dx = x - cx;
	const int dy = y - cy;
	return (dx * dx) + (dy * dy) <= (radius * radius);
}

bool CDlgImage::CalcCircumscribedCircle(
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double& centerX, double& centerY, double& radius)
{
	const double det = 2.0 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
	if (std::fabs(det) < 1e-10)
	{
		return false;
	}

	const double x1Sq = (x1 * x1) + (y1 * y1);
	const double x2Sq = (x2 * x2) + (y2 * y2);
	const double x3Sq = (x3 * x3) + (y3 * y3);

	centerX = (x1Sq * (y2 - y3) + x2Sq * (y3 - y1) + x3Sq * (y1 - y2)) / det;
	centerY = (x1Sq * (x3 - x2) + x2Sq * (x1 - x3) + x3Sq * (x2 - x1)) / det;
	radius = std::sqrt(((x1 - centerX) * (x1 - centerX)) + ((y1 - centerY) * (y1 - centerY)));
	return true;
}

void CDlgImage::DrawFilledCircle(
	unsigned char* bits,
	int pitch,
	int width,
	int height,
	int centerX,
	int centerY,
	int radius,
	unsigned char color)
{
	for (int y = centerY - radius; y <= centerY + radius; ++y)
	{
		if (y < 0 || y >= height)
		{
			continue;
		}

		for (int x = centerX - radius; x <= centerX + radius; ++x)
		{
			if (x < 0 || x >= width)
			{
				continue;
			}

			if (IsInCircle(x, y, centerX, centerY, radius))
			{
				bits[(y * pitch) + x] = color;
			}
		}
	}
}

void CDlgImage::DrawCircleOutline(
	unsigned char* bits,
	int pitch,
	int width,
	int height,
	double centerX,
	double centerY,
	double radius,
	int thickness,
	unsigned char color)
{
	const double halfThickness = static_cast<double>(thickness) / 2.0;
	double innerRadius = radius - halfThickness;
	if (innerRadius < 0.0)
	{
		innerRadius = 0.0;
	}
	const double outerRadius = radius + halfThickness;
	const double innerSq = innerRadius * innerRadius;
	const double outerSq = outerRadius * outerRadius;

	const int left = static_cast<int>(std::floor(centerX - outerRadius - 1.0));
	const int right = static_cast<int>(std::ceil(centerX + outerRadius + 1.0));
	const int top = static_cast<int>(std::floor(centerY - outerRadius - 1.0));
	const int bottom = static_cast<int>(std::ceil(centerY + outerRadius + 1.0));

	for (int y = top; y <= bottom; ++y)
	{
		if (y < 0 || y >= height)
		{
			continue;
		}

		for (int x = left; x <= right; ++x)
		{
			if (x < 0 || x >= width)
			{
				continue;
			}

			const double dx = static_cast<double>(x) - centerX;
			const double dy = static_cast<double>(y) - centerY;
			const double distSq = (dx * dx) + (dy * dy);

			if (distSq >= innerSq && distSq <= outerSq)
			{
				bits[(y * pitch) + x] = color;
			}
		}
	}
}
