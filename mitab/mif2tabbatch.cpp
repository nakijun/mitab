/**********************************************************************
 * $Id: mif2tabbatch.cpp,v 1.1 1999-11-12 00:05:26 stephane Exp $
 *
 * Name:     tab2tab.cpp
 * Project:  MapInfo TAB format Read/Write library
 * Language: C++
 * Purpose:  Copy features from a .(MID/MIF) dataset to a new one.
 * Author:   Stephane Villeneuve, s.villeneuve@videotron.ca
 *
 **********************************************************************
 * Copyright (c) 1999, Daniel Morissette
 *
 * All rights reserved.  This software may be copied or reproduced, in
 * all or in part, without the prior written consent of its author,
 * Daniel Morissette (danmo@videotron.ca).  However, any material copied
 * or reproduced must bear the original copyright notice (above), this 
 * original paragraph, and the original disclaimer (below).
 * 
 * The entire risk as to the results and performance of the software,
 * supporting text and other information contained in this file
 * (collectively called the "Software") is with the user.  Although 
 * considerable efforts have been used in preparing the Software, the 
 * author does not warrant the accuracy or completeness of the Software.
 * In no event will the author be liable for damages, including loss of
 * profits or consequential damages, arising out of the use of the 
 * Software.
 * 
 **********************************************************************
 *
 * $Log: mif2tabbatch.cpp,v $
 * Revision 1.1  1999-11-12 00:05:26  stephane
 * first revision
 *
 * Revision 1.2  1999/11/09 22:31:38  warmerda
 * initial implementation of MIF CoordSys support
 *
 * Revision 1.1  1999/11/08 19:16:22  stephane
 * first revision
 *
 * Revision 1.1  1999/11/08 04:16:07  stephane
 * First Revision
 *
 *
 **********************************************************************/


#include "mitab.h"
#include <ctype.h>

static int Mif2Tab(const char *pszSrcFname, const char *pszDstFname);


#define MIF2TAB_USAGE "Usage: Mif2TabBatch *.mif\n"

/**********************************************************************
 *                          main()
 *
 **********************************************************************/
int main(int argc, char *argv[])
{
    char *pszDstFname;
    char *pszCopy;
    
    int nLength;

/*---------------------------------------------------------------------
 *      Read program arguments.
 *--------------------------------------------------------------------*/
    if (argc<2)
    {
        printf("This program is used to perform a big number of file in the
		same time, all generated files will receive Cpy%s.tab name\n");
	printf("%s", MIF2TAB_USAGE);
        return 1;
    }
    
    for (int i=1;i<argc;i++)
    {
	nLength = strlen(argv[i]);
	nLength -=4;
	pszCopy = CPLStrdup(argv[i]);
	pszCopy[nLength] = '\0';
	printf("%s\n",pszCopy);
	pszDstFname = CPLStrdup(CPLSPrintf("Cpy%s.tab",pszCopy));
	Mif2Tab(argv[i], pszDstFname);
	CPLFree(pszCopy);
	CPLFree(pszDstFname);
    }
    return 0;
}


/**********************************************************************
 *                          Mif2Tab()
 *
 * Copy features from source dataset to a new dataset
 **********************************************************************/
static int Mif2Tab(const char *pszSrcFname, const char *pszDstFname)
{
    MIFFile  oSrcFile; 
    TABFile  oDstFile;
    int      nFeatureId;
    TABFeature *poFeature;
    double dXMin, dYMin, dXMax, dYMax;

    /*---------------------------------------------------------------------
     * Try to open source file
     *--------------------------------------------------------------------*/
    if (oSrcFile.Open(pszSrcFname, "r") != 0)
    {
        printf("Failed to open %s\n", pszSrcFname);
        return -1;
    }

    /*---------------------------------------------------------------------
     * Try to open destination file
     *--------------------------------------------------------------------*/
    if (oDstFile.Open(pszDstFname, "wb") != 0)
    {
        printf("Failed to open %s\n", pszDstFname);
        return -1;
    }

    //  Set bounds
    if (oSrcFile.GetBounds(dXMin, dYMin, dXMax, dYMax) == 0)
        oDstFile.SetBounds(dXMin, dYMin, dXMax, dYMax);

    // Pass Proj. info directly
    OGRSpatialReference *poSR;

    poSR = oSrcFile.GetSpatialRef();
    if( poSR != NULL )
    {
        oDstFile.SetSpatialRef( poSR );
    }

    /*---------------------------------------------------------------------
     * Copy objects until EOF is reached
     *--------------------------------------------------------------------*/
    nFeatureId = -1;
    while ( (nFeatureId = oSrcFile.GetNextFeatureId(nFeatureId)) != -1 )
    {
        poFeature = oSrcFile.GetFeatureRef(nFeatureId);
        if (poFeature)
        {
//            poFeature->DumpReadable(stdout);
//            poFeature->DumpMIF();
            oDstFile.SetFeature(poFeature);
        }
        else
            break;      // GetFeatureRef() failed: Abort the loop
    }

    /*---------------------------------------------------------------------
     * Cleanup and exit.
     *--------------------------------------------------------------------*/
    oDstFile.Close();

    oSrcFile.Close();

    return 0;
}

