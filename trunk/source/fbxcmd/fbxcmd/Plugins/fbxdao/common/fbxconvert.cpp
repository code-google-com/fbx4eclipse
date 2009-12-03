#include "StdAfx.h"
#include "Common.h"
#include "fbxconvert.h"

const unsigned int IntegerInf = 0x7f7fffff;
const unsigned int IntegerNegInf = 0xff7fffff;
const float FloatINF = *(float*)&IntegerInf;
const float FloatNegINF = *(float*)&IntegerNegInf;

void PosRotScaleNode(KFbxNode *n, KFbxXMatrix& m3, PosRotScale prs)
{

	KFbxVector4 p = m3.GetT();
	KFbxQuaternion q = m3.GetQ();
	KFbxXMatrix stm = m3 * m3.Inverse();
	//float s = (sqrt(stm.GetRow(0)[0]) + sqrt(stm.GetRow(1)[1]) + sqrt(stm.GetRow(1)[1])) / 3.0f;
	KFbxVector4 s(stm.Get(0,0), stm.Get(1,1), stm.Get(2,2));
	PosRotScaleNode(n, p, q, s, prs);
}

// Set Position and Rotation on a standard controller will need to handle bipeds
//   Always in World Transform coordinates
void PosRotScaleNode(KFbxNode *n, KFbxVector4 & p, KFbxQuaternion& q, float s, PosRotScale prs)
{
	if (prs & prsScale && s == FloatNegINF) prs = PosRotScale(prs & ~prsScale);

	KFbxVector4 sv(s, s, s);
	PosRotScaleNode(n, p, q, sv, prs);
}

void PosRotScaleNode(KFbxNode *n, KFbxVector4 & p, KFbxQuaternion& q, KFbxVector4 & s, PosRotScale prs)
{
	if (prs & prsRot && q.GetAt(3) == FloatNegINF) prs = PosRotScale(prs & ~prsRot);
	if (prs & prsPos && p.GetAt(0) == FloatNegINF) prs = PosRotScale(prs & ~prsPos);

	if (prs & prsScale) {
		n->SetDefaultS(s);
		//if (KFCurve *sclCtrl = n->LclScaling.GetKFCurve(KFCURVENODE_S_X))
		//	sclCtrl->SetValue(s.GetAt(0));
		//if (KFCurve *sclCtrl = n->LclScaling.GetKFCurve(KFCURVENODE_S_Y))
		//	sclCtrl->SetValue(s.GetAt(1));
		//if (KFCurve *sclCtrl = n->LclScaling.GetKFCurve(KFCURVENODE_S_Z))
		//	sclCtrl->SetValue(s.GetAt(2));
	}
	if (prs & prsRot){
		KFbxXMatrix qm;
		qm.SetQ(q);
		n->SetDefaultR(qm.GetR());
		//if (KFCurve *rotCtrl = n->LclRotation.GetKFCurve(KFCURVENODE_R_X))
		//	rotCtrl->SetValue(q.GetAt(0));
		//if (KFCurve *rotCtrl = n->LclRotation.GetKFCurve(KFCURVENODE_R_Y))
		//	rotCtrl->SetValue(q.GetAt(1));
		//if (KFCurve *rotCtrl = n->LclRotation.GetKFCurve(KFCURVENODE_R_Z))
		//	rotCtrl->SetValue(q.GetAt(2));
		//if (KFCurve *rotCtrl = n->LclRotation.GetKFCurve(KFCURVENODE_R_W))
		//	rotCtrl->SetValue(q.GetAt(3));
	}
	if (prs & prsPos){
		n->SetDefaultT(p);
		//if (KFCurve *posCtrl = n->LclTranslation.GetKFCurve(KFCURVENODE_T_X))
		//	posCtrl->SetValue(p.GetAt(0));
		//if (KFCurve *posCtrl = n->LclTranslation.GetKFCurve(KFCURVENODE_T_Y))
		//	posCtrl->SetValue(p.GetAt(1));
		//if (KFCurve *posCtrl = n->LclTranslation.GetKFCurve(KFCURVENODE_T_Z))
		//	posCtrl->SetValue(p.GetAt(2));
	}
}
