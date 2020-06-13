#include "linkboard.h"

#include <QThread>
#include <QScreen>
#include <QPixmap>
#include <QRect>
#include <QDir>
#include <QGuiApplication>
#include <QMessagebox>
#include <board/boardfactory.h>
#include <QMutex>

#include "mainwindow.h"

Chess::LinkBoard::LinkBoard(MainWindow* pMain, QString catName)
	:m_pMain(pMain),
	m_catName(catName)
{
	this->initBoard();   
}

void Chess::LinkBoard::runAutoChess()
{
	this->bMustStop = false;
	bool bWeMustSendInitFen = false;

	this->m_Ready_LXset = true;

	if (this->m_Ready_LXset == false) {
		this->GetLxInfo(this->m_catName);
	}



}

bool Chess::LinkBoard::GetLxInfo(QString catlog)
{
	this->m_Ready_LXset = false;
	//stLxBoard* pPieceList = &m_LxBoard[0];


	
	return false;
}

// ���ݳ���ͼ���õ����̾��
bool Chess::LinkBoard::getChessboardHwnd(bool onlyBChe)
{
	LPCWSTR pClass = NULL;
	LPCWSTR pCaption = NULL;

	//if(m_Parentclass != "")
	if (m_Parentclass != "") {
		pClass = reinterpret_cast <LPCWSTR>(m_Parentclass.data());
	}
	if (m_ParentKeyword != "") {
		pCaption = reinterpret_cast <LPCWSTR>(m_ParentKeyword.data());
	}
	
	HWND hParent = ::FindWindow(pClass, pCaption);
	if (!hParent) {
		return false;
	}

	pClass = NULL;
	pCaption = NULL;

	if (m_class != "" || m_titleKeyword != "") {
		if (m_class != "") {
			pClass = reinterpret_cast <LPCWSTR>(m_class.data());
		}
		if (m_titleKeyword != "") {
			pCaption = reinterpret_cast <LPCWSTR>(m_titleKeyword.data());
		}

		HWND hSub = ::FindWindowEx(hParent, NULL, pClass, pCaption);
		if (!hSub) {
			return false;
		}
		this->m_hwnd = hSub;
	}
	else {
		this->m_hwnd = hParent;
	}

	// ��ȷ��һ�£��ǲ����ж����ڳ�
	if (onlyBChe) {
		if (!searchChess(this->m_hwnd, "br.png", m_LxBoard[0].BCheList, Side::Black, true)) {
			return false;
		}
		if (m_LxBoard[0].BCheList.count() == 2) {
			return true;
		}
	}
	else {
		if (!searchChess(this->m_hwnd, "bk.png", m_LxBoard[0].BKingList, Side::Black, true)) {
			return false;
		}

		if (!searchChess(this->m_hwnd, "rk.png", m_LxBoard[0].RKingList, Side::White)) {
			return false;
		}

		if (m_LxBoard[0].RKingList.count() == 1 && m_LxBoard[0].BKingList.count() == 1) {
			return true;  // ���Ӱѽ���ʱ�䷽�ˣ���
		}
	}	
	return false;
}

bool Chess::LinkBoard::Board2Move(Chess::GenericMove& m)
{
	ChinesePieceType* pre_board = this->m_LxBoard[0].b90;
	ChinesePieceType* now_board = this->m_LxBoard[1].b90;

	int fx = -1;
	int fy = -1;
	int tx = -1;
	int ty = -1;


	for (int y = 0; y < 10; y++) {
		for (int x = 0; x < 9; x++) {
			ChinesePieceType chess2 = pre_board[y * 9 + x];
			ChinesePieceType chess1 = now_board[y * 9 + x];

			if (chess2 != chess1) {     // �������߶�
				if (chess1 == ChinesePieceType::eNoPice) {       // 
					fx = x;
					fy = y;
				}
				else {
					tx = x;
					ty = y;
				}
			}
		}
	}

	if (fx != -1 && fy != -1 && tx != -1 && ty != -1) {
		int from = fy * 9 + fx;
		int to = ty * 9 + tx;


		//if (m_flip) {
		//	from = (9 - fy) * 9 + (8-fx);
		//	to = (9 - ty) * 9 + (8-tx);
		//}


		if (from != to) {

			ChinesePieceType piece = pre_board[from];
			if (piece <= ChinesePieceType::eBKing && piece >= ChinesePieceType::eBPawn) {
				if (m_side != Chess::Side::Black) {
					return false;
				}
			}
			else if (piece <= ChinesePieceType::eRKing && piece >= ChinesePieceType::eRPawn) {
				if (m_side != Chess::Side::White) {
					return false;
				}
			}
			else {
				//this->SendMessageToMain("error", "move error");
				return false;
			}

			//if (m_flip) {
			//	fx = 8 - fx;
			//	fy = 9 - fy;
			//	tx = 8 - tx;
			//	ty = 9 - ty;
			//}

			Square from_square = Square(fx, 9 - fy);
			Square to_square = Square(tx, 9 - ty);


			m = Chess::GenericMove(from_square, to_square);

			if (m_pMain->isMoveValid(m) == true) {
				// ����
				this->m_LxBoard[0].b90[from] = ChinesePieceType::eNoPice;
				this->m_LxBoard[0].b90[to] = piece;



				//m = 0x331e;

				//int source = Chess::sq

				//Square(int file, int rank);
				//
				//Chess::GenericMove gmove;
				//gmove.
				//
				//m = Chess::Move(from, to);
				return true;

			}

		}
	}

	return false;
}

bool Chess::LinkBoard::GetFen(stLxBoard* pList)
{
	//int b90[90] = { 0 };
		//int a = sizeof(pList->b90);
	memset(pList->b90, 0, sizeof(pList->b90));


	//bool flip = false;
	if (pList->BKingList[0].y > pList->RKingList[0].y) {
		m_flip = true;
	}
	else {
		m_flip = false;
	}

	fillB90(pList->b90, pList->RCheList, ChinesePieceType::eRChe);
	fillB90(pList->b90, pList->RMaList, ChinesePieceType::eRMa);
	fillB90(pList->b90, pList->RPaoList, ChinesePieceType::eRPao);
	fillB90(pList->b90, pList->RShiList, ChinesePieceType::eRShi);
	fillB90(pList->b90, pList->RXiangList, ChinesePieceType::eRXiang);
	fillB90(pList->b90, pList->RPawnList, ChinesePieceType::eRPawn);
	fillB90(pList->b90, pList->RKingList, ChinesePieceType::eRKing);

	fillB90(pList->b90, pList->BCheList, ChinesePieceType::eBChe);
	fillB90(pList->b90, pList->BMaList, ChinesePieceType::eBMa);
	fillB90(pList->b90, pList->BPaoList, ChinesePieceType::eBPao);
	fillB90(pList->b90, pList->BShiList, ChinesePieceType::eBShi);
	fillB90(pList->b90, pList->BXiangList, ChinesePieceType::eBXiang);
	fillB90(pList->b90, pList->BPawnList, ChinesePieceType::eBPawn);
	fillB90(pList->b90, pList->BKingList, ChinesePieceType::eBKing);

	// getFen from B90


	QString fen;
	QChar c;

	for (int rank = 0; rank <= 9; rank++) {
		for (int file = 0; file <= 8; ) {
			int s90 = file + rank * 9;
			ChinesePieceType chess = pList->b90[s90];

			if (chess == ChinesePieceType::eNoPice) {
				int len = 0;
				for (; file <= 8 && pList->b90[file + rank * 9] == ChinesePieceType::eNoPice; file++) {
					len++;
				}
				c = '0' + len;
			}
			else {
				c = Qpiece_to_char(chess);
				file++;
			}
			fen += c;
		}
		fen += (rank < 9 ? '/' : ' ');
	}
	fen += (m_flip == false ? "w " : "b ");   // ���fen ֻ��һ��
	fen += "- - 0 1";

	pList->fen = fen;

	return true;
}

bool Chess::LinkBoard::fillB90(ChinesePieceType b90[], QVector<cv::Point>& plist, ChinesePieceType chess)
{
	for (auto p : plist) {
		int s90 = getB90(p);
		b90[s90] = chess;
	}

	return true;
}

int Chess::LinkBoard::getB90(cv::Point p)
{
	int dx = (int)((p.x - m_offx) / m_dx + 0.5f);
	int dy = (int)((p.y - m_offy) / m_dy + 0.5f);

	if (m_flip) {
		dy = 9 - dy;
		dx = 8 - dx;
	}

	int s90 = dx + 9 * dy;

	if (s90 > 89) s90 = 89;
	return s90;
}

QChar Chess::LinkBoard::Qpiece_to_char(ChinesePieceType chess)
{
	static const  QChar PieceString[]
		= { '0','p','b','a','c','n','r','k','P','B','A','C','N','R','K' };
	return PieceString[(int)chess];
}

bool Chess::LinkBoard::SaveAllPiecePicture(QString subCat)
{
	if (subCat == nullptr) {
		subCat = "0";                     // ��һ������
	}

	// Bche
	SaveOnePiecePic(0, 0, "br.png",subCat);
	SaveOnePiecePic(1, 0, "bn.png",subCat);
	SaveOnePiecePic(2, 0, "bb.png",subCat);
	SaveOnePiecePic(3, 0, "ba.png",subCat);
	SaveOnePiecePic(4, 0, "bk.png",subCat);
								 
	SaveOnePiecePic(1, 2, "bc.png",subCat);
	SaveOnePiecePic(0, 3, "bp.png",subCat);
								  
	// �쳵						
	SaveOnePiecePic(0, 9, "rr.png",subCat);
	SaveOnePiecePic(1, 9, "rn.png",subCat);
	SaveOnePiecePic(2, 9, "rb.png",subCat);
	SaveOnePiecePic(3, 9, "ra.png",subCat);
	SaveOnePiecePic(4, 9, "rk.png",subCat);
								 
	SaveOnePiecePic(1, 7, "rc.png",subCat);  // ����
	SaveOnePiecePic(0, 6, "rp.png",subCat);

	return true;
}

bool Chess::LinkBoard::SaveOnePiecePic(int x, int y, QString chessName, QString subCat)
{
	// �õ�ȫĿ¼
	try {
		QString runPath = QCoreApplication::applicationDirPath();
		QString picFile = runPath + "/image/linkboard/" + this->m_catName + "/chess/" + subCat + "/" + chessName;

		int pieceSize = int(m_dx * 0.8f);
		QRect rect(
			m_offx - pieceSize / 2 + x * m_dx,
			m_offy - pieceSize / 2 + y * m_dx,
			pieceSize, pieceSize);
		QPixmap cropped = this->m_capPixmap.copy(rect);
		cropped.save(picFile, "PNG");   // �ڳ�
	}
	catch (...) {
		return false;
	}
	return true;
}

void Chess::LinkBoard::initBoard()
{
	if (m_catName == "��������") {        // ��������
		m_precision_chess = 0.57f; // was 0.52
		m_precision_auto = 0.98f;
		m_UseAdb = false;
		m_sleepTimeMs = 20;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		m_Ready_LXset = false;
		m_chessWinOK = false;

		//m_chessClip = 0.25f;

		//this->m_board = Chess::BoardFactory::create("standard");
		//this->m_board_second = Chess::BoardFactory::create("standard");
		//m_board->reset();
		//m_board_second->reset();


		//this->m_LxInfo.m_LX_name = "��������С����";
		m_ParentKeyword = "";           // �����ڹؼ���
	    m_Parentclass = "";             // ��������
		m_titleKeyword = "�й�����2017"; // "BHGUI(test) - �����";
		m_class = "QQChess"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";

		//this->m_LxInfo.offx = 29.0f;
		//this->m_LxInfo.offy = 138.0f;
		//this->m_LxInfo.m_dx = 28.0f;
		//this->m_LxInfo.m_dy = 28.0f;

		m_offx = 73.0f;
		m_offy = 175.0f;
		m_dx = 68.0f;
		m_dy = 68.0f;

		this->m_Ready_LXset = false;

		//this->m_LxInfo.m_PieceCatlog = "0";
		this->m_connectedBoard_OK = false;  // 

		m_side = Chess::Side::NoSide;

		m_flip = false;

		m_iLowHred = 0;
		m_iHighHred = 10;
		
		m_iLowSred = 77;
		m_iHighSred = 255;
		
		m_iLowVred = 95;
		m_iHighVred = 255;
	
		m_iLowHblack = 0;
		m_iHighHblack = 51;
		
		m_iLowSblack = 0;
		m_iHighSblack = 90;
		
		m_iLowVblack = 0;
		m_iHighVblack = 140;
	}
	else if (m_catName == "��������") {   // ��������
		m_precision_chess = 0.57f; // was 0.52
		m_precision_auto = 0.98f;
		m_UseAdb = false;
		m_sleepTimeMs = 20;
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;

		m_Ready_LXset = false;
		m_chessWinOK = false;

		//m_chessClip = 0.25f;

		//this->m_board = Chess::BoardFactory::create("standard");
		//this->m_board_second = Chess::BoardFactory::create("standard");
		//m_board->reset();
		//m_board_second->reset();

		m_ParentKeyword = "��������";                 // �����ڹؼ���
		m_Parentclass = "#32770 (�Ի���)";             // ��������

		//this->m_LxInfo.m_LX_name = "��������С����";
		m_titleKeyword = "Chrome Legacy Window"; // "BHGUI(test) - �����";
		m_class = "Chrome_RenderWidgetHostHWND"; // "Afx:00400000:b:00010003:00000006:0A1D05FB";

		//this->m_LxInfo.offx = 29.0f;
		//this->m_LxInfo.offy = 138.0f;
		//this->m_LxInfo.m_dx = 28.0f;
		//this->m_LxInfo.m_dy = 28.0f;

		m_offx = 37.0f;
		m_offy = 41.0f;
		m_dx = 50.0f;
		m_dy = 50.0f;

		this->m_Ready_LXset = false;

		//this->m_LxInfo.m_PieceCatlog = "1";
		//this->m_connectedBoard_OK = false;  // 

		m_side = Chess::Side::NoSide;

		m_flip = false;


		m_iLowHred = 0;
		m_iHighHred = 10;

		m_iLowSred = 77;
		m_iHighSred = 255;

		m_iLowVred = 95;
		m_iHighVred = 255;

		m_iLowHblack = 0;
		m_iHighHblack = 179;

		m_iLowSblack = 0;
		m_iHighSblack = 139;

		m_iLowVblack = 0;
		m_iHighVblack = 131;
	}

}

void Chess::LinkBoard::winLeftClick(HWND hwnd, int x, int y, int off_x, int off_y)
{
	LONG temp = MAKELONG(x+off_x, y+off_y);
	::SendMessage(hwnd, WM_LBUTTONDOWN, 0, temp);
	wait(1);
	::SendMessage(hwnd, WM_LBUTTONUP, 0, temp);
}

bool Chess::LinkBoard::searchCountours(HWND hw, QString findName, bool isCap)
{
	if (isCap) {
		if (this->captureOne(hw) == false) {
			qWarning("searchImage 1 %s �����ˣ�", findName);
			return false;
		}
	}

	cv::namedWindow("Control", cv::WINDOW_AUTOSIZE); //create a window called "Control"
	int iLowH = 100;
	int iHighH = 140;

	int iLowS = 90;
	int iHighS = 255;

	int iLowV = 90;
	int iHighV = 255;

	//Create trackbars in "Control" window		

	cv::createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cv::createTrackbar("HighH", "Control", &iHighH, 179);

	cv::createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cv::createTrackbar("HighS", "Control", &iHighS, 255);

	cv::createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cv::createTrackbar("HighV", "Control", &iHighV, 255);


	while (true) {

		cv::Mat imgHSV;
		std::vector<cv::Mat> hsvSplit;
		cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

		//cv::split(imgHSV, hsvSplit);
		//cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
		//cv::merge(hsvSplit, imgHSV);

		cv::Mat imgThresholded;

		cv::inRange(imgHSV, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		// ������ (ȥ��һЩ���)
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
		cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_OPEN, element);

		// �ղ��� (����һЩ��ͨ��)
		cv::morphologyEx(imgThresholded, imgThresholded, cv::MORPH_CLOSE, element);



		cv::imshow("Thresholded Image", imgThresholded); //show the thresholded image
		cv::imshow("Original", m_image_source); //show the original image

		char key = (char)cv::waitKey(300);
		if (key == 27)
			break;
	}

	// opencv ʵ���ض���ɫ������ȡ�붨λ
	// https://blog.csdn.net/chenghaoy/article/details/86509950


	//CV_EXPORTS_W void findContours(InputArray image, OutputArrayOfArrays contours,
	//	OutputArray hierarchy, int mode,
	//	int method, Point offset = Point());

	///** @overload */
	//CV_EXPORTS void findContours(InputArray image, OutputArrayOfArrays contours,
	//	int mode, int method, Point offset = Point());

	//cv::Mat m_image_find = m_image_source;   // ת���õ���ͼ
	cv::Mat grayImage;
	cv::Mat out_Canny;

	cv::cvtColor(m_image_source, grayImage, cv::COLOR_BGR2GRAY);

	cv::Mat hsvImage;
	cv::cvtColor(m_image_source, hsvImage, cv::COLOR_BGR2HSV);

	int hueValue = 0; // red color
	int hueRange = 15; // how much difference from the desired color we want to include to the result If you increase this value, for example a red color would detect some orange values, too.

	//int minSaturation = 50; // I'm not sure which value is good here...
	//int minValue = 50; // not sure whether 50 is a good min value here...

	cv::Mat hueMask;
	cv::inRange(hsvImage, hueValue - hueRange, hueValue + hueRange, hueMask);


	int min_Thresh = 240;
	int max_Thresh = 255;
	Canny(grayImage, out_Canny, min_Thresh, max_Thresh * 2, 3);

	//cv::imshow("source", m_image_source);
	cv::imshow("canny", out_Canny);
	//cv::waitKey();

	//CV_RETR_EXTERNALֻ�������Χ��������������Χ�����ڵ���Χ����������
	std::vector<std::vector<cv::Point>> g_vContours;
	std::vector<cv::Vec4i> g_vHierarchy;

	cv::RNG G_RNG(1234);

	cv::findContours(out_Canny, g_vContours, g_vHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	cv::Mat Drawing = cv::Mat::zeros(out_Canny.size(), CV_8UC3);

	for (int i = 0; i < g_vContours.size(); i++) {
		cv::Scalar color = cv::Scalar(G_RNG.uniform(0, 255), G_RNG.uniform(0, 255), G_RNG.uniform(0, 255));
		cv::drawContours(Drawing, g_vContours, i, color, 2, 8, g_vHierarchy, 0);
	}

	cv::imshow("gray", grayImage);

	cv::imshow("drawing", Drawing);

	//cv::imshow("contours", grayImage);
	cv::waitKey();

	return true;
}

bool Chess::LinkBoard::searchImage(QString findName, bool isCap, QString sub_catlog, float threshold)
{
	int imgX, imgY;
	if (sub_catlog == nullptr) {
		sub_catlog = "/auto/";
	}
	return searchImage(findName, isCap, sub_catlog, imgX, imgY, threshold);
}

bool Chess::LinkBoard::searchImage(QString findName, bool isCap, QString sub_catlog, int& imgX, int& imgY, float threshold)
{
	if (isCap) {
		if (this->captureOne(m_hwnd, false) == false) {
			qWarning("searchImage 11 %s �����ˣ�", qUtf8Printable(findName));
			return false;
		}
	}

	if (sub_catlog != nullptr) {
		findName = sub_catlog + findName;
	}

	//QString runPath = QCoreApplication::applicationDirPath();
	//QString picFile = runPath + "/" + this->m_catName + "/chess/" + subCat + "/" + chessName;

	cv::Mat image_template_main;
	try {

		if (this->m_MatHash.contains(findName)) {              // �����ڻ�����			
			image_template_main = this->m_MatHash.value(findName);
		}
		else {
			QString runPath = QCoreApplication::applicationDirPath() + "/image/linkboard/" + this->m_catName;
			QString fFile = runPath + findName;
			image_template_main = cv::imread(fFile.toLocal8Bit().toStdString());   // ģ��ͼ
			// Ҫ��Ҫ����
			//if (this->m_scaleX != 1.0f) {
			//	cv::resize(image_template_main, image_template_main, cv::Size(), this->m_scaleX, this->m_scaleY);
			//}
			this->m_MatHash.insert(findName, image_template_main);
		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s �����ˣ�", findName);
		return false;
	}


	//cv::imshow("templ", m_image_black);
	//cv::imshow("img", m_image_source);
	//cv::imshow("matched", image_template2);
	//cv::waitKey();	

	cv::Mat image_matched;
	try {
		cv::matchTemplate(m_image_source_all, image_template_main, image_matched, cv::TM_SQDIFF_NORMED); // cv::TM_CCORR); // cv::TM_SQDIFF);
	}
	catch (...) {
		qWarning("searchImage 3 %s �����ˣ�", qUtf8Printable(findName));
		return false;
	}

#if 0
	cv::imshow("m_image_source", m_image_source);
	//cv::imshow("templ", image_template2);
	cv::imshow("m_image_black", m_image_red);
	cv::imshow("matched", image_matched);
	cv::waitKey();
#endif
	try {

		if (threshold == 1.0f) {
			threshold = this->m_precision_auto;
		}

		cv::Point minLoc, maxLoc;
		double minVal, maxVal;

		//Ѱ�����ƥ��λ��
		cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);

		double matchThres = maxVal * (1 - threshold);

		if (minVal < matchThres) {
			imgX = minLoc.x + image_template_main.cols / 2;
			imgY = minLoc.y + image_template_main.rows / 2;
			return true;
		}
		else {
			return false;
		}
	}
	catch (...) {
		//qWarning("searchImage 5 %s �����ˣ�", findName);
		return false;
	}


	return false;
}

bool Chess::LinkBoard::captureOne(HWND hw, bool isTransHSV, int sleepTimeMs)
{
	if (this->m_UseAdb) {
		//return this->pAdb->one_screenshot(this->capPixmap, disp, fname, sleepTimeMs, path);
		return false;
	}
	else {
		return this->CaptureOneNotry(hw, sleepTimeMs, isTransHSV);
	}
}

bool Chess::LinkBoard::CaptureOneNotry(HWND hw, int sleepTimeMS, bool isTransHSV)
{
	try {

		QThread::msleep(sleepTimeMS);

		QScreen* screen = QGuiApplication::primaryScreen();

		this->m_capPixmap = screen->grabWindow(WId(hw));

		//fname = "tmp2.png";
		//if (fname != nullptr) {
		//	this->m_capPixmap.save(path + fname, "PNG");
		//}

		//QString cname = this->get_window_class(hw);

		// ת���� openCV ��ʽ
		/*
		cv::Mat mat = QImage_to_cvMat(this->m_capPixmap.toImage(), false);
		m_image_source = cv::Mat(mat.rows, mat.cols, CV_8UC3);
		int from_to[] = { 0,0,  1,1,  2,2 };
		cv::mixChannels(&mat, 1, &m_image_source, 1, from_to, 3);
		*/

		if (this->m_capPixmap.width() < 100) {   // ͼ̫С��
			return false;
		}

		if (m_isAutoClick) {
			m_image_source_all = QPixmapToCvMat(this->m_capPixmap, true);
			m_image_source = m_image_source_all;
		}
		else {
			this->m_image_source = QPixmapToCvMat(this->m_capPixmap, true);
		}


		if (this->m_Ready_LXset) {  // ��ǰ��������ϢOK��

			//this->m_LxInfo.m_dx

			cv::Rect crect(0, 0, m_offx + m_dx * 9, m_offy + m_dx * 10);

			m_image_source = m_image_source(crect);

			//cv::imshow("templ", m_image_source);
			//cv::waitKey();
		}

		// ��ֺ�ڶ���ͼ
		//cv::Mat m_image_red;      // �췽����
		//cv::Mat m_image_black;    // �ڷ�����
		if (isTransHSV) {
			cv::Mat imgHSV;
			cv::cvtColor(m_image_source, imgHSV, cv::COLOR_BGR2HSV);

			cv::inRange(imgHSV, cv::Scalar(m_iLowHred, m_iLowSred, m_iLowVred),
				cv::Scalar(m_iHighHred, m_iHighSred, m_iHighVred), m_imgage_SHV[Side::White]); //Threshold the image
			cv::inRange(imgHSV, cv::Scalar(m_iLowHblack, m_iLowSblack, m_iLowVblack),
				cv::Scalar(m_iHighHblack, m_iHighSblack, m_iHighVblack), m_imgage_SHV[Side::Black]); //Threshold the image

			// ��ֵ������
			//cv::threshold(m_image_red, m_image_red, 125, 255, cv::THRESH_BINARY);
			//cv::threshold(m_image_black, m_image_black, 125, 255, cv::THRESH_BINARY);
		}

		//cv::Mat hsv_channels_red[3];
		//cv::Mat hsv_red;
		//cv::inRange(imgHSV, cv::Scalar(iLowHred, iLowSred, iLowVred), cv::Scalar(iHighHred, iHighSred, iHighVred), hsv_red); //Threshold the image
		//cv::split(hsv_red, hsv_channels_red);
		//m_image_red = hsv_channels_red[2];


		//cv::split(hsv_red, hsv_channels_red);
		//m_image_red = hsv_channels_red[2];

		//cv::inRange(imgHSV, cv::Scalar(iLowHblack, iLowSblack, iLowVblack), cv::Scalar(iHighHblack, iHighSblack, iHighVblack), m_image_black); //Threshold the image

		/*
		cv::imshow("m_image_red", m_image_red); //show the thresholded image
		cv::imshow("m_image_black", m_image_black); //show the original image
		cv::waitKey();
		*/


		// isDisplay	
		//if (this->displayCapture) {
		//	GOB::DisplayImage(this->capPixmap, this->isZoom);
		//}

	}
	catch (...) {
		//qWarning("��ͼ�����ˣ� %s", path + fname);
		return false;
	}
	return  true;
}

bool Chess::LinkBoard::searchChess(HWND hw, QString findName, QVector<cv::Point>& res, Chess::Side side, bool isCap, float threshold, bool isShow)
{
	if (isCap) {
		if (this->captureOne(hw) == false) {
			qWarning("searchImage 1 %s �����ˣ�", qUtf8Printable(findName));
			return false;
		}
	}
	bool Isfind = false;
	bool bIsInHash = false;

	QVector<cv::Mat> template_chess;

	try {
		if (this->m_MatHash.contains(findName)) {              // �����ڻ�����
			template_chess << this->m_MatHash.value(findName);
			bIsInHash = true;
		}
		else {  // �����еĸ������ӷ���
			QString filePath = QCoreApplication::applicationDirPath() + "/image/linkboard/" + this->m_catName + "/chess/";
			QDir dir(filePath);
			if (!dir.exists()) {
				return false;
			}
			dir.setFilter(QDir::Dirs); // | QDir::Files | QDir::NoDotAndDotDot);
			//�ļ�������
			dir.setSorting(QDir::DirsFirst);
			//QFileInfoList list = dir.entryInfoList();
			QStringList files = dir.entryList();
			if (files.size() < 1) {
				return false;
			}
			for (QString file : files) {
				QString chessFile = filePath + file + findName;
				//cv::Mat mtemp = cv::imread(chessFile.toLocal8Bit().toStdString());   // ģ��ͼ
				cv::Mat imgHSV;
				cv::cvtColor(cv::imread(chessFile.toLocal8Bit().toStdString()), imgHSV, cv::COLOR_BGR2HSV);

				cv::Mat mtemp;
				if (side == Side::White) {
					cv::inRange(imgHSV, cv::Scalar(m_iLowHblack, m_iLowSblack, m_iLowVblack),
						cv::Scalar(m_iHighHblack, m_iHighSblack, m_iHighVblack), mtemp); //Threshold the image
				}
				else {
					cv::inRange(imgHSV, cv::Scalar(m_iLowHred, m_iLowSred, m_iLowVred),
						cv::Scalar(m_iHighHred, m_iHighSred, m_iHighVred), mtemp); //Threshold the image
				}
				template_chess << mtemp;
			}

		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s �����ˣ�", findName);
		return false;
	}

	cv::TemplateMatchModes method = cv::TM_SQDIFF_NORMED;  // cv::TM_CCOEFF_NORMED; // cv::TM_SQDIFF_NORMED;
	try {
		for (cv::Mat m : template_chess) {
			cv::Mat image_matched;
			cv::matchTemplate(this->m_imgage_SHV[side], m, image_matched, method);

#if 0
			cv::imshow("m_image_source", m_image_source);
			//cv::imshow("templ", image_template2);
			cv::imshow("m_image_black", m_image_red);
			cv::imshow("matched", image_matched);
			cv::waitKey();
#endif
			if (threshold == 1.0f) {
				threshold = this->m_precision_chess;
			}
			res.clear();  // �������

			while (true) {
				cv::Point minLoc, maxLoc;
				double minVal, maxVal;

				//Ѱ�����ƥ��λ��
				cv::minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);


				//double matchVal = minVal / tmpSize;  // ȥ��ģ���С��ƥ�侫�ȵ�Ӱ��
				//threshold = 0.90;
				double matchThres = maxVal * (1.0 - threshold);
				if (minVal < matchThres) {

					//if(maxVal > 0.75){

					Isfind = true;
					

					cv::Point chessP;

					chessP.x = minLoc.x + m.cols / 2;
					chessP.y = minLoc.y + m.rows / 2;


					if (isShow) {
						//cv::Mat image_color;
						//cv::cvtColor(image_source, image_color, cv::CV_BGR2GRAY);
						cv::circle(m_image_source,
							chessP, //cv::Point(imgX, imgY),
							40,
							cv::Scalar(0, 255, 255),
							2,
							8,
							0);

						cv::imshow("target", m_image_source);
						//cv::imshow("templ", image_template);
					}

					res.append(chessP);

					//cv::Rect rc; // = cv::Rect(minLoc, cv::Size(10, 10));
					//cv::imshow("m1", image_matched);
					//cv::floodFill(image_matched, minLoc, cv::Scalar(maxVal), &rc, cv::Scalar(minVal * 0.5), cv::Scalar(minVal * 0.5));

					cv::circle(image_matched, minLoc, 10, cv::Scalar(maxVal), -1);

					//cv::circle(image_matched, maxLoc, 10, cv::Scalar(minVal), -1);

					//cv::imshow("m2", image_matched);
					//cv::waitKey();
				}
				else {
					break;
				}

			}

			if (Isfind) {
				if (bIsInHash == false) {					
					this->m_MatHash.insert(findName, m);		
				}
				return true;
			}
		}
	}
	catch (...) {
		//qWarning("searchImage 2 %s �����ˣ�", findName);
		return false;
	}
	return false;
}

//bool Chess::LinkBoard::SearchOnChessList(HWND hwnd, QString chess, QVector<cv::Point>& res, bool IsCap)
//{
//	//if (isCap) {
//	//	if (this->captureOne(hw) == false) {
//	//		qWarning("searchImage 1 %s �����ˣ�", qUtf8Printable(findName));
//	//		return false;
//	//	}
//	//}
//
//
//
//
//	return false;
//
//	
//}

cv::Mat Chess::LinkBoard::QImageToCvMat(const QImage& inImage, bool inCloneImageData)
{
	switch (inImage.format())
	{
		// 8-bit, 4 channel
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC4,
			const_cast<uchar*>(inImage.bits()),
			static_cast<size_t>(inImage.bytesPerLine())
		);

		return (inCloneImageData ? mat.clone() : mat);
	}

	// 8-bit, 3 channel
	case QImage::Format_RGB32:
	case QImage::Format_RGB888:
	{
		if (!inCloneImageData)
		{
			qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
		}

		QImage   swapped = inImage;

		if (inImage.format() == QImage::Format_RGB32)
		{
			swapped = swapped.convertToFormat(QImage::Format_RGB888);
		}

		swapped = swapped.rgbSwapped();

		return cv::Mat(swapped.height(), swapped.width(),
			CV_8UC3,
			const_cast<uchar*>(swapped.bits()),
			static_cast<size_t>(swapped.bytesPerLine())
		).clone();
	}

	// 8-bit, 1 channel
	case QImage::Format_Indexed8:
	{
		cv::Mat  mat(inImage.height(), inImage.width(),
			CV_8UC1,
			const_cast<uchar*>(inImage.bits()),
			static_cast<size_t>(inImage.bytesPerLine())
		);

		return (inCloneImageData ? mat.clone() : mat);
	}

	default:
		qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
		break;
	}

	return cv::Mat();
	// https ://blog.csdn.net/qq_17550379/article/details/78683153
}

cv::Mat Chess::LinkBoard::QPixmapToCvMat(const QPixmap& inPixmap, bool inCloneImageData)
{
	return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

QString Chess::LinkBoard::get_window_title(HWND hwnd)
{
	QString retStr;
	wchar_t temp[512];
	int len;

	len = GetWindowTextLengthW(hwnd);
	if (!len)
		return retStr;

	//temp = reinterpret_cast<wchar_t *>(malloc(sizeof(wchar_t) * (quint32(len)+1)));
	if (GetWindowTextW(hwnd, temp, len + 1))
	{
		retStr = QString::fromWCharArray(temp);
	}
	//free(temp);
	return retStr;
}

QString Chess::LinkBoard::get_window_class(HWND hwnd)
{
	QString retStr;
	wchar_t temp[256];

	temp[0] = 0;
	if (GetClassNameW(hwnd, temp, sizeof(temp) / sizeof(wchar_t)))
	{
		retStr = QString::fromWCharArray(temp);
	}
	return retStr;
}