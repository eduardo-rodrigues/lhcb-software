// $Id: ITLayer.cpp,v 1.1.1.1 2004-01-29 15:57:39 mneedham Exp $
//
// This File contains the definition of the ITLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

//_________________________________________________
// ITLayer
// ST layer properties 

#include "STDet/ITLayer.h"
#include "Kernel/ITChannelID.h"
#include "STDet/STWafer.h"


ITLayer::ITLayer(int stationID, int layerID, double z, 
                    double stereoAngle, double pitch, double waferWidth, 
		    double waferHeight, double waferThickness,
		    double waferOverlap, unsigned int wafersX, 
		    unsigned int wafersY, double holeX, double holeY, 
		    double ladderDist,
		     unsigned int numStrips, int position):
  STDetectionLayer(stationID, layerID, z, 
                   stereoAngle, pitch,waferThickness,
                   ladderDist)
{

  // constructer
  m_WafersNum = 2*wafersX;
  m_Wafers.resize(m_WafersNum);

  double sensWaferWidth = pitch*(double)numStrips;
  double guardRingSize = 0.5*(waferWidth -sensWaferWidth);
  double ladderHeight = (double)wafersY*waferHeight;
  double sensLadderHeight = ((double)wafersY*waferHeight)-(2.0*guardRingSize);

  int iBox;
  unsigned int currWafer = 1;
  unsigned int currStrip = 0;

  if (position == CENTER_POS){
    m_type = "TB";
    // sensor height and y position  
    double yCenterBox = holeY+((ladderHeight*cosAngle())
                               +(waferWidth*fabs(sinAngle())))/2.0;

      for (iBox= -1; iBox < 2; iBox += 2 ){     
        double yWafer = (double)iBox*yCenterBox;
        for (unsigned int cWafer = 1; cWafer<= wafersX; cWafer++){

          double xWafer = ((waferWidth-waferOverlap)/cosAngle())
	   *((double)cWafer-((double)wafersX+1.)/2.);
          double dz = (1.-(2.*(cWafer%2)))*0.5*ladderDist;    

          double uWafer = xWafer*cosAngle() + yWafer*sinAngle();
          double vWafer = yWafer*cosAngle() - xWafer*sinAngle();
 
          m_Wafers[currWafer-1] = new STWafer(pitch, 1,wafersY, 
	 				    stationID,layerID,currWafer,
                                            uWafer - sensWaferWidth/2.0,  
                                            uWafer + sensWaferWidth/2.0, 
			                    vWafer - sensLadderHeight/2.0, 
			                    vWafer + sensLadderHeight/2.0, 
                                            dz,guardRingSize);

         currStrip += m_Wafers[currWafer-1]->lastStrip();
         currWafer++;      

        } // iWafer
      } // boxes
  }
  else {

    // has to be side
    m_type = "LR";
    double yWafer = 0.;

    for (iBox= -1; iBox < 2; iBox += 2 ){
      for (unsigned int sWafer = 1; sWafer<= wafersX; sWafer++){
        double xWafer = holeX 
	  + ((wafersX-sWafer)*(waferWidth-waferOverlap)/cosAngle())
          + (0.5*waferWidth*cosAngle())  
          + (0.5*ladderHeight*fabs(sinAngle()));

	// sign....
	xWafer *= (double)iBox ; 
        double dz = (1.-(2.*(sWafer%2)))*0.5*ladderDist;
       
        double uWafer = xWafer*cosAngle() + yWafer*sinAngle();
        double vWafer = yWafer*cosAngle() - xWafer*sinAngle();

        unsigned int waferId = sWafer; 
        if (iBox == 1){
          waferId = (2u*wafersX)-sWafer+1;
	}

        m_Wafers[waferId-1u] = new STWafer(pitch, 1,wafersY, 
				            stationID,layerID,waferId,
                                            uWafer - sensWaferWidth/2.0,  
                                            uWafer + sensWaferWidth/2.0, 
			                    vWafer - sensLadderHeight/2.0, 
			                    vWafer + sensLadderHeight/2.0, 
                                            dz,guardRingSize);

       currStrip += m_Wafers[waferId-1u]->lastStrip();        
      

      } // sWafer 
    } // iBox
  }

  m_totNumStrips = currStrip; 
}

ITLayer::~ITLayer(){
  //destructer

  if (m_Wafers.size() != 0) {
    std::vector<STWafer*>::iterator iWafer = m_Wafers.begin();
    while (iWafer != m_Wafers.end()) {
      STWafer* aWafer = *iWafer;
      delete aWafer;
      m_Wafers.erase(iWafer);
    }
  }
}

double ITLayer::centerX(const ITChannelID stripID) const {
  return (this->U(stripID) - this->centerY(stripID)*sinAngle())/cosAngle();
}

double ITLayer::centerY(const ITChannelID stripID) const {

  double yC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    yC = v*cosAngle() + u*sinAngle();
  }

  return yC;
}

double ITLayer::centerZ(const ITChannelID stripID) const {
  // z center of channel

  double zC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
      zC = this->z() + m_Wafers[waferNum-1]->dZ();
  } 
  return zC;
}

double ITLayer::halfLengthX(const ITChannelID stripID) const {
  return -this->halfLengthY(stripID)*sinAngle()/cosAngle();
}

double ITLayer::halfLengthY(const ITChannelID stripID) const {
  const unsigned int stripNum = stripID.strip();
  for (unsigned int iWafer = 1; iWafer <= m_WafersNum; iWafer++) {
    if (m_Wafers[iWafer-1]->isStrip(stripNum)) {
      return m_Wafers[iWafer-1]->halfLengthV()*cosAngle();
    }
  }
  return 0.;
}

unsigned int ITLayer::numStrips() const {
  return m_totNumStrips;
}














