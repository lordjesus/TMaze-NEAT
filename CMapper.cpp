#include "CMapper.h"



//--------------------------- Init ---------------------------------------
//
//  This method needs to be called before you can use the instance.
//------------------------------------------------------------------------
void CMapper::Init(int MaxRangeX, int MaxRangeY)
{
	//if already initialized return
	if(m_NumCellsX) return;

	m_dCellSize = CParams::dCellSize;

	//first calculate how many segments we will require
	m_NumCellsX = (int)(MaxRangeX/m_dCellSize)+1;
	m_NumCellsY = (int)(MaxRangeY/m_dCellSize)+1;

	//create the 2d vector of blank segments
	for (int x=0; x<m_NumCellsX; ++x)
	{
		vector<SCell> temp;

		for (int y=0; y<m_NumCellsY; ++y)
		{
			temp.push_back(SCell(x*m_dCellSize, (x+1)*m_dCellSize, y*m_dCellSize, (y+1)*m_dCellSize));
		}

		m_2DvecCells.push_back(temp);
	}

	m_iTotalCells = m_NumCellsX * m_NumCellsY;

}
//-------------------------------------------------------------
void CMapper::Update(double xPos, double yPos)
{
	//check to make sure positions are within range
	if ( (xPos < 0) || (xPos > CParams::WindowWidth) || 
		(yPos < 0) || (yPos > CParams::WindowHeight) )
	{
		return;
	}

	int cellX = (int)(xPos / m_dCellSize );
	int cellY = (int)(yPos / m_dCellSize );

	m_2DvecCells[cellX][cellY].Update();

	return;
}

bool CMapper::CheckReward(double xPos, double yPos) 
{
	//check to make sure positions are within range
	if ( (xPos < 0) || (xPos > CParams::WindowWidth) || 
		(yPos < 0) || (yPos > CParams::WindowHeight) )
	{
		return false;
	}

	int cellX = (int)(xPos / m_dCellSize );
	int cellY = (int)(yPos / m_dCellSize );

	if (cellX < 50 / m_dCellSize) {
		return true;
	}

	if (cellX > m_NumCellsX - (70 / m_dCellSize)) {
		return true;
	}

	return false;
}

//---------------------------------------------------------------
int CMapper::TicksLingered(double xPos, double yPos)const
{
	//bounds check the incoming values
	if ( (xPos > CParams::WindowWidth) || (xPos < 0) ||
		(yPos > CParams::WindowHeight)|| (yPos < 0))
	{
		return 999;
	}

	int cellX = (int)(xPos / m_dCellSize);
	int cellY = (int)(yPos / m_dCellSize);

	return m_2DvecCells[cellX][cellY].iTicksSpentHere;
}

//------------------------- Visited --------------------------------------
//
//------------------------------------------------------------------------
bool CMapper::BeenVisited(double xPos, double yPos)const
{
	int cellX = (int)(xPos / m_dCellSize);
	int cellY = (int)(yPos / m_dCellSize);

	if (m_2DvecCells[cellX][cellY].iTicksSpentHere > 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}
//--------------------------------- Render -------------------------------
//
//  renders the visited cells. The color gets darker the more frequently
//  the cell has been visited.
//------------------------------------------------------------------------
void CMapper::Render(HDC surface)
{

	for (int x=0; x<m_NumCellsX; ++x)
	{
		for (int y=0; y<m_NumCellsY; ++y)
		{
			if (m_2DvecCells[x][y].iTicksSpentHere > 0)
			{
				int shading = 2 * m_2DvecCells[x][y].iTicksSpentHere;

				if (shading >220)
				{
					shading = 220;
				}


				HBRUSH lightbrush = CreateSolidBrush(RGB(240,220-shading,220-shading));

				FillRect(surface, &m_2DvecCells[x][y].Cell, lightbrush); 

				DeleteObject(lightbrush);
			}
		}
	} 
}

void CMapper::RenderReward(HDC surface) 
{
	for (int x=0; x<m_NumCellsX; ++x)
	{
		for (int y= 280 / m_dCellSize; y<m_NumCellsY; ++y)
		{
			if (x < 50/m_dCellSize) 
			{			

				HBRUSH lightbrush = CreateSolidBrush(RGB(200, 200, 200));

				FillRect(surface, &m_2DvecCells[x][y].Cell, lightbrush); 

				DeleteObject(lightbrush);
			}
			else if (x > m_NumCellsX - (70 / m_dCellSize)) {
				
				HBRUSH lightbrush = CreateSolidBrush(RGB(100, 100, 100));

				FillRect(surface, &m_2DvecCells[x][y].Cell, lightbrush); 

				DeleteObject(lightbrush);
			}
		}
	} 
}

//----------------------------------- Reset ------------------------------
void CMapper::Reset()
{

	for (int x=0; x<m_NumCellsX; ++x)
	{
		for (int y=0; y<m_NumCellsY; ++y)
		{
			m_2DvecCells[x][y].Reset();
		}
	}
}

double CMapper::TMazeReward() {
	double reward = 0;

outer: for (int x = 0; x < m_NumCellsX; x++) {
	for (int y = 0; y < m_NumCellsY; y++) {
		// Inefficient - I know
		if (x < 50/m_dCellSize) {
			// Left leg
			if (m_2DvecCells[x][y].iTicksSpentHere > 0) {
				reward += m_2DvecCells[x][y].iTicksSpentHere;

			}
		} else if (x > m_NumCellsX - (70 / m_dCellSize)) {
			// Right leg
			if (m_2DvecCells[x][y].iTicksSpentHere > 0) {
				reward += 5 * m_2DvecCells[x][y].iTicksSpentHere;

			}
		}
	}
	   }
stop:

	   return reward;
}

int CMapper::NumCellsVisited() const
{
	int total = 0;

	for (int x=0; x<m_NumCellsX; ++x)
	{
		for (int y=0; y<m_NumCellsY; ++y)
		{
			if (m_2DvecCells[x][y].iTicksSpentHere > 0)
			{
				++total;
			}
		}
	}

	return total;
}


