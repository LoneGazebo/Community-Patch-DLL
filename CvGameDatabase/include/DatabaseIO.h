
#pragma once 

namespace Database
{
	class BinaryIO
	{
	public:
		CvGameDatabaseAPI BinaryIO(const char* szFilename);

		CvGameDatabaseAPI bool operator << (Connection& db);
		CvGameDatabaseAPI bool operator >> (Connection& db);

		CvGameDatabaseAPI bool Save(Connection& db);
		CvGameDatabaseAPI bool Load(Connection& db);

	protected:
		bool SaveLoad(const char* szFilename, Connection& db, bool bSave);

	private:
		const char* m_pszFilename;
	};

}	//namespace Database