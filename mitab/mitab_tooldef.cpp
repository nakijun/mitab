/**********************************************************************
 * $Id: mitab_tooldef.cpp,v 1.1 1999-09-26 14:59:37 daniel Exp $
 *
 * Name:     mitab_tooldef.cpp
 * Project:  MapInfo TAB Read/Write library
 * Language: C++
 * Purpose:  Implementation of the TABToolDefTable class used to handle
 *           a dataset's table of drawing tool blocks
 * Author:   Daniel Morissette, danmo@videotron.ca
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
 * $Log: mitab_tooldef.cpp,v $
 * Revision 1.1  1999-09-26 14:59:37  daniel
 * Implemented write support
 *
 **********************************************************************/

#include "mitab.h"
#include "mitab_utils.h"

/*=====================================================================
 *                      class TABToolDefTable
 *====================================================================*/

/**********************************************************************
 *                   TABToolDefTable::TABToolDefTable()
 *
 * Constructor.
 **********************************************************************/
TABToolDefTable::TABToolDefTable()
{
    m_papsPen = NULL;
    m_papsBrush = NULL;
    m_papsFont = NULL;
    m_papsSymbol = NULL;
    m_numPen = 0;
    m_numBrushes = 0;
    m_numFonts = 0;
    m_numSymbols = 0;
    m_numAllocatedPen = 0;
    m_numAllocatedBrushes = 0;
    m_numAllocatedFonts = 0;
    m_numAllocatedSymbols = 0;

}

/**********************************************************************
 *                   TABToolDefTable::~TABToolDefTable()
 *
 * Destructor.
 **********************************************************************/
TABToolDefTable::~TABToolDefTable()
{
    int i;

    for(i=0; m_papsPen && i < m_numPen; i++)
        CPLFree(m_papsPen[i]);
    CPLFree(m_papsPen);

    for(i=0; m_papsBrush && i < m_numBrushes; i++)
        CPLFree(m_papsBrush[i]);
    CPLFree(m_papsBrush);

    for(i=0; m_papsFont && i < m_numFonts; i++)
        CPLFree(m_papsFont[i]);
    CPLFree(m_papsFont);

    for(i=0; m_papsSymbol && i < m_numSymbols; i++)
        CPLFree(m_papsSymbol[i]);
    CPLFree(m_papsSymbol);

}


/**********************************************************************
 *                   TABToolDefTable::ReadAllToolDefs()
 *
 * Read all tool definition blocks until we reach the end of the chain.
 * This function will be called only once per dataset, after that
 * we keep all the tool definitions in memory.
 *
 * Returns 0 on success, -1 on error.
 **********************************************************************/
int     TABToolDefTable::ReadAllToolDefs(TABMAPToolBlock *poBlock)
{
    int nStatus = 0;
    int nDefType;

    /*-----------------------------------------------------------------
     * Loop until we reach the end of the chain of blocks... we assume
     * that the first block of data is already pre-loaded. 
     *----------------------------------------------------------------*/
    while( ! poBlock->EndOfChain() )
    {
        nDefType = poBlock->ReadByte();
        switch(nDefType)
        {
          case 1:       // PEN
            if (m_numPen >= m_numAllocatedPen)
            {
                // Realloc array by blocks of 20 items
                m_numAllocatedPen += 20;
                m_papsPen = (TABPenDef**)CPLRealloc(m_papsPen, 
                                        m_numAllocatedPen*sizeof(TABPenDef*));
            }
            m_papsPen[m_numPen] = (TABPenDef*)CPLCalloc(1, sizeof(TABPenDef));

            m_papsPen[m_numPen]->nRefCount  = poBlock->ReadInt32();
            m_papsPen[m_numPen]->nLineWidth = poBlock->ReadByte();
            m_papsPen[m_numPen]->nLinePattern = poBlock->ReadByte();
            m_papsPen[m_numPen]->nLineStyle = poBlock->ReadByte();
            m_papsPen[m_numPen]->rgbColor   = poBlock->ReadByte()*256*256+
                                              poBlock->ReadByte()*256 + 
                                              poBlock->ReadByte();

            m_numPen++;

            break;
          case 2:       // BRUSH
            if (m_numBrushes >= m_numAllocatedBrushes)
            {
                // Realloc array by blocks of 20 items
                m_numAllocatedBrushes += 20;
                m_papsBrush = (TABBrushDef**)CPLRealloc(m_papsBrush, 
                                 m_numAllocatedBrushes*sizeof(TABBrushDef*));
            }
            m_papsBrush[m_numBrushes] = 
                               (TABBrushDef*)CPLCalloc(1,sizeof(TABBrushDef));

            m_papsBrush[m_numBrushes]->nRefCount    = poBlock->ReadInt32();
            m_papsBrush[m_numBrushes]->nFillPattern = poBlock->ReadByte();
            m_papsBrush[m_numBrushes]->bTransparentFill = poBlock->ReadByte();
            m_papsBrush[m_numBrushes]->rgbFGColor =poBlock->ReadByte()*256*256+
                                                   poBlock->ReadByte()*256 + 
                                                   poBlock->ReadByte();
            m_papsBrush[m_numBrushes]->rgbBGColor =poBlock->ReadByte()*256*256+
                                                   poBlock->ReadByte()*256 + 
                                                   poBlock->ReadByte();

            m_numBrushes++;

            break;
          case 3:       // FONT NAME
            if (m_numFonts >= m_numAllocatedFonts)
            {
                // Realloc array by blocks of 20 items
                m_numAllocatedFonts += 20;
                m_papsFont = (TABFontDef**)CPLRealloc(m_papsFont, 
                                 m_numAllocatedFonts*sizeof(TABFontDef*));
            }
            m_papsFont[m_numFonts] = 
                               (TABFontDef*)CPLCalloc(1,sizeof(TABFontDef));

            m_papsFont[m_numFonts]->nRefCount    = poBlock->ReadInt32();
            poBlock->ReadBytes(32, (GByte*)m_papsFont[m_numFonts]->szFontName);
            m_papsFont[m_numFonts]->szFontName[32] = '\0';

            m_numFonts++;

            break;
          case 4:       // SYMBOL
            if (m_numSymbols >= m_numAllocatedSymbols)
            {
                // Realloc array by blocks of 20 items
                m_numAllocatedSymbols += 20;
                m_papsSymbol = (TABSymbolDef**)CPLRealloc(m_papsSymbol, 
                                 m_numAllocatedSymbols*sizeof(TABSymbolDef*));
            }
            m_papsSymbol[m_numSymbols] = 
                               (TABSymbolDef*)CPLCalloc(1,sizeof(TABSymbolDef));

            m_papsSymbol[m_numSymbols]->nRefCount    = poBlock->ReadInt32();
            m_papsSymbol[m_numSymbols]->nSymbolNo    = poBlock->ReadInt16();
            m_papsSymbol[m_numSymbols]->nPointSize   = poBlock->ReadInt16();
            m_papsSymbol[m_numSymbols]->_nUnknownValue_ = poBlock->ReadByte();
            m_papsSymbol[m_numSymbols]->rgbColor = poBlock->ReadByte()*256*256+
                                                   poBlock->ReadByte()*256 + 
                                                   poBlock->ReadByte();

            m_numSymbols++;

            break;
          default:
            /* Unsupported Tool type!!! */
            CPLError(CE_Failure, CPLE_NotSupported,
                     "Unsupported drawing tool type: `%d'", nDefType);
            nStatus = -1;
        }

        if (CPLGetLastErrorNo() != 0)
        {
            // An error happened reading this tool definition... stop now.
            nStatus = -1;
        }
    }

    return nStatus;
}


/**********************************************************************
 *                   TABToolDefTable::WriteAllToolDefs()
 *
 * Write all tool definition structures to the TABMAPToolBlock.
 *
 * Note that at the end of this call, poBlock->CommitToFile() will have
 * been called.
 *
 * Returns 0 on success, -1 on error.
 **********************************************************************/
int     TABToolDefTable::WriteAllToolDefs(TABMAPToolBlock *poBlock)
{
    int i, nStatus = 0;

    /*-----------------------------------------------------------------
     * Write Pen Defs
     *----------------------------------------------------------------*/
    for(i=0; nStatus == 0 && i< m_numPen; i++)
    {
        poBlock->WriteByte(1);  // Def Type = Pen
        poBlock->WriteInt32(m_papsPen[i]->nRefCount);

        poBlock->WriteByte(m_papsPen[i]->nLineWidth);
        poBlock->WriteByte(m_papsPen[i]->nLinePattern);
        poBlock->WriteByte(m_papsPen[i]->nLineStyle);
        poBlock->WriteByte(COLOR_R(m_papsPen[i]->rgbColor));
        poBlock->WriteByte(COLOR_G(m_papsPen[i]->rgbColor));
        poBlock->WriteByte(COLOR_B(m_papsPen[i]->rgbColor));

        if (CPLGetLastErrorNo() != 0)
        {
            // An error happened reading this tool definition... stop now.
            nStatus = -1;
        }
    }

    /*-----------------------------------------------------------------
     * Write Brush Defs
     *----------------------------------------------------------------*/
    for(i=0; nStatus == 0 && i< m_numBrushes; i++)
    {
        poBlock->WriteByte(2);  // Def Type = Brush
        poBlock->WriteInt32(m_papsBrush[i]->nRefCount);

        poBlock->WriteByte(m_papsBrush[i]->nFillPattern);
        poBlock->WriteByte(m_papsBrush[i]->bTransparentFill);
        poBlock->WriteByte(COLOR_R(m_papsBrush[i]->rgbFGColor));
        poBlock->WriteByte(COLOR_G(m_papsBrush[i]->rgbFGColor));
        poBlock->WriteByte(COLOR_B(m_papsBrush[i]->rgbFGColor));
        poBlock->WriteByte(COLOR_R(m_papsBrush[i]->rgbBGColor));
        poBlock->WriteByte(COLOR_G(m_papsBrush[i]->rgbBGColor));
        poBlock->WriteByte(COLOR_B(m_papsBrush[i]->rgbBGColor));

        if (CPLGetLastErrorNo() != 0)
        {
            // An error happened reading this tool definition... stop now.
            nStatus = -1;
        }
    }

    /*-----------------------------------------------------------------
     * Write Font Defs
     *----------------------------------------------------------------*/
    for(i=0; nStatus == 0 && i< m_numFonts; i++)
    {
        poBlock->WriteByte(3);  // Def Type = Font name
        poBlock->WriteInt32(m_papsFont[i]->nRefCount);

        poBlock->WriteBytes(32, (GByte*)m_papsFont[i]->szFontName);

        if (CPLGetLastErrorNo() != 0)
        {
            // An error happened reading this tool definition... stop now.
            nStatus = -1;
        }
    }

    /*-----------------------------------------------------------------
     * Write Symbol Defs
     *----------------------------------------------------------------*/
    for(i=0; nStatus == 0 && i< m_numSymbols; i++)
    {
        poBlock->WriteByte(4);  // Def Type = Symbol
        poBlock->WriteInt32(m_papsSymbol[i]->nRefCount);

        poBlock->WriteInt16(m_papsSymbol[i]->nSymbolNo);
        poBlock->WriteInt16(m_papsSymbol[i]->nPointSize);
        poBlock->WriteByte(m_papsSymbol[i]->_nUnknownValue_);
        poBlock->WriteByte(COLOR_R(m_papsSymbol[i]->rgbColor));
        poBlock->WriteByte(COLOR_G(m_papsSymbol[i]->rgbColor));
        poBlock->WriteByte(COLOR_B(m_papsSymbol[i]->rgbColor));

        if (CPLGetLastErrorNo() != 0)
        {
            // An error happened reading this tool definition... stop now.
            nStatus = -1;
        }
    }

    if (nStatus == 0)
        nStatus = poBlock->CommitToFile();

    return nStatus;
}



/**********************************************************************
 *                   TABToolDefTable::GetNumPen()
 *
 * Return the number of valid pen indexes for this .MAP file
 **********************************************************************/
int     TABToolDefTable::GetNumPen()
{
    return m_numPen;
}

/**********************************************************************
 *                   TABToolDefTable::GetPenDefRef()
 *
 * Return a reference to the specified Pen tool definition, or NULL if
 * specified index is invalid.
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
TABPenDef *TABToolDefTable::GetPenDefRef(int nIndex)
{
    if (nIndex >0 && nIndex <= m_numPen)
        return m_papsPen[nIndex-1];

    return NULL;
}

/**********************************************************************
 *                   TABToolDefTable::AddPenDefRef()
 *
 * Either create a new PenDefRef or add a reference to an existing one.
 *
 * Return the pen index that has been attributed to this Pen tool 
 * definition, or -1 if something went wrong
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
int TABToolDefTable::AddPenDefRef(TABPenDef *poNewPenDef)
{
    int i, nNewPenIndex = 0;
    TABPenDef *poDef;

    if (poNewPenDef == NULL)
        return -1;

    /*-----------------------------------------------------------------
     * Start by searching the list of existing pens
     *----------------------------------------------------------------*/
    for (i=0; nNewPenIndex == 0 && i<m_numPen; i++)
    {
        poDef = m_papsPen[i];
        if (poDef->nLineWidth == poNewPenDef->nLineWidth &&
            poDef->nLinePattern == poNewPenDef->nLinePattern &&
            poDef->nLineStyle == poNewPenDef->nLineStyle &&
            poDef->rgbColor == poNewPenDef->rgbColor)
        {
            nNewPenIndex = i+1; // Fount it!
            poDef->nRefCount++;
        }               
    }

    /*-----------------------------------------------------------------
     * OK, we did not find a match, then create a new entry
     *----------------------------------------------------------------*/
    if (nNewPenIndex == 0)
    {
        if (m_numPen >= m_numAllocatedPen)
        {
            // Realloc array by blocks of 20 items
            m_numAllocatedPen += 20;
            m_papsPen = (TABPenDef**)CPLRealloc(m_papsPen, 
                                       m_numAllocatedPen*sizeof(TABPenDef*));
        }
        m_papsPen[m_numPen] = (TABPenDef*)CPLCalloc(1, sizeof(TABPenDef));

        *m_papsPen[m_numPen] = *poNewPenDef;
        m_papsPen[m_numPen]->nRefCount = 1;
        nNewPenIndex = ++m_numPen;
    }

    return nNewPenIndex;
}

/**********************************************************************
 *                   TABToolDefTable::GetNumBrushes()
 *
 * Return the number of valid Brush indexes for this .MAP file
 **********************************************************************/
int     TABToolDefTable::GetNumBrushes()
{
    return m_numBrushes;
}

/**********************************************************************
 *                   TABToolDefTable::GetBrushDefRef()
 *
 * Return a reference to the specified Brush tool definition, or NULL if
 * specified index is invalid.
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
TABBrushDef *TABToolDefTable::GetBrushDefRef(int nIndex)
{
    if (nIndex >0 && nIndex <= m_numBrushes)
        return m_papsBrush[nIndex-1];

    return NULL;
}

/**********************************************************************
 *                   TABToolDefTable::AddBrushDefRef()
 *
 * Either create a new BrushDefRef or add a reference to an existing one.
 *
 * Return the Brush index that has been attributed to this Brush tool 
 * definition, or -1 if something went wrong
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
int TABToolDefTable::AddBrushDefRef(TABBrushDef *poNewBrushDef)
{
    int i, nNewBrushIndex = 0;
    TABBrushDef *poDef;

    if (poNewBrushDef == NULL)
        return -1;

    /*-----------------------------------------------------------------
     * Start by searching the list of existing Brushs
     *----------------------------------------------------------------*/
    for (i=0; nNewBrushIndex == 0 && i<m_numBrushes; i++)
    {
        poDef = m_papsBrush[i];
        if (poDef->nFillPattern == poNewBrushDef->nFillPattern &&
            poDef->bTransparentFill == poNewBrushDef->bTransparentFill &&
            poDef->rgbFGColor == poNewBrushDef->rgbFGColor &&
            poDef->rgbBGColor == poNewBrushDef->rgbBGColor)
        {
            nNewBrushIndex = i+1; // Fount it!
            poDef->nRefCount++;
        }               
    }

    /*-----------------------------------------------------------------
     * OK, we did not find a match, then create a new entry
     *----------------------------------------------------------------*/
    if (nNewBrushIndex == 0)
    {
        if (m_numBrushes >= m_numAllocatedBrushes)
        {
            // Realloc array by blocks of 20 items
            m_numAllocatedBrushes += 20;
            m_papsBrush = (TABBrushDef**)CPLRealloc(m_papsBrush, 
                                 m_numAllocatedBrushes*sizeof(TABBrushDef*));
        }
        m_papsBrush[m_numBrushes]=(TABBrushDef*)CPLCalloc(1, 
                                                          sizeof(TABBrushDef));

        *m_papsBrush[m_numBrushes] = *poNewBrushDef;
        m_papsBrush[m_numBrushes]->nRefCount = 1;
        nNewBrushIndex = ++m_numBrushes;
    }

    return nNewBrushIndex;
}

/**********************************************************************
 *                   TABToolDefTable::GetNumFonts()
 *
 * Return the number of valid Font indexes for this .MAP file
 **********************************************************************/
int     TABToolDefTable::GetNumFonts()
{
    return m_numFonts;
}

/**********************************************************************
 *                   TABToolDefTable::GetFontDefRef()
 *
 * Return a reference to the specified Font tool definition, or NULL if
 * specified index is invalid.
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
TABFontDef *TABToolDefTable::GetFontDefRef(int nIndex)
{
    if (nIndex >0 && nIndex <= m_numFonts)
        return m_papsFont[nIndex-1];

    return NULL;
}

/**********************************************************************
 *                   TABToolDefTable::AddFontDefRef()
 *
 * Either create a new FontDefRef or add a reference to an existing one.
 *
 * Return the Font index that has been attributed to this Font tool 
 * definition, or -1 if something went wrong
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
int TABToolDefTable::AddFontDefRef(TABFontDef *poNewFontDef)
{
    int i, nNewFontIndex = 0;
    TABFontDef *poDef;

    if (poNewFontDef == NULL)
        return -1;

    /*-----------------------------------------------------------------
     * Start by searching the list of existing Fonts
     *----------------------------------------------------------------*/
    for (i=0; nNewFontIndex == 0 && i<m_numFonts; i++)
    {
        poDef = m_papsFont[i];
        if (EQUAL(poDef->szFontName, poNewFontDef->szFontName))
        {
            nNewFontIndex = i+1; // Fount it!
            poDef->nRefCount++;
        }               
    }

    /*-----------------------------------------------------------------
     * OK, we did not find a match, then create a new entry
     *----------------------------------------------------------------*/
    if (nNewFontIndex == 0)
    {
        if (m_numFonts >= m_numAllocatedFonts)
        {
            // Realloc array by blocks of 20 items
            m_numAllocatedFonts += 20;
            m_papsFont = (TABFontDef**)CPLRealloc(m_papsFont, 
                                 m_numAllocatedFonts*sizeof(TABFontDef*));
        }
        m_papsFont[m_numFonts]=(TABFontDef*)CPLCalloc(1, 
                                                          sizeof(TABFontDef));

        *m_papsFont[m_numFonts] = *poNewFontDef;
        m_papsFont[m_numFonts]->nRefCount = 1;
        nNewFontIndex = ++m_numFonts;
    }

    return nNewFontIndex;
}

/**********************************************************************
 *                   TABToolDefTable::GetNumSymbols()
 *
 * Return the number of valid Symbol indexes for this .MAP file
 **********************************************************************/
int     TABToolDefTable::GetNumSymbols()
{
    return m_numSymbols;
}

/**********************************************************************
 *                   TABToolDefTable::GetSymbolDefRef()
 *
 * Return a reference to the specified Symbol tool definition, or NULL if
 * specified index is invalid.
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
TABSymbolDef *TABToolDefTable::GetSymbolDefRef(int nIndex)
{
    if (nIndex >0 && nIndex <= m_numSymbols)
        return m_papsSymbol[nIndex-1];

    return NULL;
}


/**********************************************************************
 *                   TABToolDefTable::AddSymbolDefRef()
 *
 * Either create a new SymbolDefRef or add a reference to an existing one.
 *
 * Return the Symbol index that has been attributed to this Symbol tool 
 * definition, or -1 if something went wrong
 *
 * Note that nIndex is a 1-based index.  A value of 0 indicates "none" 
 * in MapInfo.
 **********************************************************************/
int TABToolDefTable::AddSymbolDefRef(TABSymbolDef *poNewSymbolDef)
{
    int i, nNewSymbolIndex = 0;
    TABSymbolDef *poDef;

    if (poNewSymbolDef == NULL)
        return -1;

    /*-----------------------------------------------------------------
     * Start by searching the list of existing Symbols
     *----------------------------------------------------------------*/
    for (i=0; nNewSymbolIndex == 0 && i<m_numSymbols; i++)
    {
        poDef = m_papsSymbol[i];
        if (poDef->nSymbolNo == poNewSymbolDef->nSymbolNo &&
            poDef->nPointSize == poNewSymbolDef->nPointSize &&
            poDef->_nUnknownValue_ == poNewSymbolDef->_nUnknownValue_ &&
            poDef->rgbColor == poNewSymbolDef->rgbColor )
        {
            nNewSymbolIndex = i+1; // Fount it!
            poDef->nRefCount++;
        }               
    }

    /*-----------------------------------------------------------------
     * OK, we did not find a match, then create a new entry
     *----------------------------------------------------------------*/
    if (nNewSymbolIndex == 0)
    {
        if (m_numSymbols >= m_numAllocatedSymbols)
        {
            // Realloc array by blocks of 20 items
            m_numAllocatedSymbols += 20;
            m_papsSymbol = (TABSymbolDef**)CPLRealloc(m_papsSymbol, 
                                 m_numAllocatedSymbols*sizeof(TABSymbolDef*));
        }
        m_papsSymbol[m_numSymbols]=(TABSymbolDef*)CPLCalloc(1, 
                                                       sizeof(TABSymbolDef));

        *m_papsSymbol[m_numSymbols] = *poNewSymbolDef;
        m_papsSymbol[m_numSymbols]->nRefCount = 1;
        nNewSymbolIndex = ++m_numSymbols;
    }

    return nNewSymbolIndex;
}
