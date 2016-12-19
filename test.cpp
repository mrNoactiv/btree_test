﻿#include "dstruct/paged/b+tree/cB+Tree.h"
#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cDataCollection.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/core/cBulkLoad.h"

using namespace common::random;
using namespace common::data;

const unsigned int ITEM_COUNT = 2000000;
const unsigned int DOMAIN_MAX = ITEM_COUNT;    //  Maximum value for random generator in cTuplesGenerator.

const unsigned int CACHE_SIZE = 20000;
const unsigned int BLOCK_SIZE = /* 2048; */ 8192;
const unsigned int MAX_NODE_INMEM_SIZE = 1.25 * BLOCK_SIZE;

// defines the length of the inserting tuples
const unsigned int TUPLE_LENGTHS[] = { 5 };

// defines the length of the inserting data
const unsigned int DATA_LENGTHS[] = { 4 };

// defines the type of data structure mode
const unsigned int DSMODES[] = { cDStructConst::DSMODE_DEFAULT, cDStructConst::DSMODE_CODING,cDStructConst::DSMODE_RI, cDStructConst::DSMODE_RICODING };
//const unsigned int DSMODES[] = { cDStructConst::DSMODE_RI };

// defines type of tuple
const unsigned int ITEM_TYPES[] = { cDStructConst::TUPLE, cDStructConst::NTUPLE };
//const unsigned int ITEM_TYPES[] = { cDStructConst::TUPLE };

// defines the type of coding in the case of CODING and RICODING
unsigned int CODETYPE = FIXED_LENGTH_CODING_ALIGNED;
// ELIAS_DELTA 1		FIBONACCI2 2		FIBONACCI3  3		ELIAS_FIBONACCI 4
// ELIAS_DELTA_FAST 5	FIBONACCI2_FAST 6	FIBONACCI3_FAST 7	ELIAS_FIBONACCI_FAST 8
// FIXED_LENGTH_CODING 9	FIXED_LENGTH_CODING_ALIGNED 10

// defines maximal compression ratio in RI, CODING and RICODING mode
unsigned int COMPRESSION_RATIO = 2;

unsigned int MULTIPLERANGEQUERY = false;
unsigned int QUERIES_IN_MULTIQUERY = 4; // number of queries in multiquery

unsigned int RUNTIME_MODE = cDStructConst::RTMODE_DEFAULT; // defines the mode (with or without validation) RTMODE_DEFAULT		RTMODE_VALIDATION

// Histograms for each dimension will be created during build
bool HISTOGRAMS = false;

static const uint START_SEED = 865915;					// defines the starting seed of data generator
static const uint INMEMCACHE_SIZE = 0; // 1048576;

char dbPath[1024] = "quickdb";

typedef cNTuple tKey_VarLen;
typedef cTuple tKey_FixedLen;

typedef cTuple TKey;
typedef cBpTree<TKey> BpTree;

typedef cUInt tDomain; // defines domain used for loading tuples from collection file.

typedef cBulkLoad<BpTree, cBpTreeNode<TKey>, cBpTreeNode<TKey>, TKey, cBpTreeHeader<TKey>, tDomain> tBulkload;

uint DSMODE, ITEM_TYPE, TUPLE_LENGTH, DATA_LENGTH;

cTuplesGenerator<tDomain, tKey_VarLen>* generator_VarLen;

cTuplesGenerator<tDomain, tKey_FixedLen>* generator_FixedLen;
cTuplesGenerator<tDomain, TKey>* tuples_generatorCheck;


//cSpaceDescriptor<cUInt, cInt> sD; generika


char *mData_FixedLen;


tKey_FixedLen *mAuxTuple1;
tKey_FixedLen *mAuxTuple2;

/// specification of collections ///
int COLLECTION = cCollection::RANDOM;
int COMPUTER = Computer::DBEDU;
char *collectionFile, *queryFile;
uint nofDeletedItems, nofNotFoundItems;


void printHeader();
void InitializeTuplesGenerator(uint type);
void CloseTuplesGenerator(uint type);

void Create_VariableLen(bool variableData);
bool Open_VariableLen();

void Create_FixedLen();
bool Open_FixedLen();

void Bulkloading_FixedLen();

void checkItem_variablelen(cBpTree<tKey_VarLen> *index, unsigned int j);
bool checkItem_fixedlen(cBpTree<tKey_FixedLen> *index, unsigned int j);

uint DSMODES_COUNT() { return sizeof(DSMODES) / sizeof(uint); };
uint ITEM_TYPES_COUNT() { return sizeof(ITEM_TYPES) / sizeof(uint); };
uint TUPLE_LENGTHS_COUNT() { return sizeof(TUPLE_LENGTHS) / sizeof(uint); };
uint DATA_LENGTHS_COUNT() { return sizeof(DATA_LENGTHS) / sizeof(uint); };

int main()
{
	/*
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	*/
	
	cout << "hue";
	
	int i = 11;
	int j = 2;
	char *c = new char();
	
	cSpaceDescriptor * SD1 = new cSpaceDescriptor(5, new tKey_FixedLen(), new cInt(), false);
	cSpaceDescriptor * SD2 = new cSpaceDescriptor(5, new tKey_FixedLen(), new cInt(), false);

	tKey_FixedLen* cT = new cTuple(SD1,SD2);

	
	cT->SetValue(3, j, SD1);//nefunguje
	cT->SetValue(0, i, SD2);//funguje
	
	cT->GetValue(cT->GetData(), SD1);
	cT->GetValue(cT->GetData(), SD2);


	//--------------------------------------------------------------------------
	char *ch = new char();
	
	
	
	
	cSpaceDescriptor * SD = new cSpaceDescriptor(5, new tKey_FixedLen(), new cUInt(), false);
	tKey_FixedLen* cT2 = new cTuple(SD);


	cT2->SetValue(0, i, SD);
	cT2->GetValue(ch, SD);



	//cSpaceDescriptor<>



	//generator_FixedLen = new cTuplesGenerator<cUInt, tKey_FixedLen>(ITEM_COUNT, TUPLE_LENGTH, DOMAIN_MAX);


	
	
	printf("\nTest successfully completed!\n");
	cin.get();
	return 0;
}

// **********************************************************************
// ********************** VarLen Items **********************************
// **********************************************************************
void Create_VariableLen(bool variableData)
{
	bool debug = false;
	cTimer runtime;
	tKey_VarLen tp(generator_VarLen->GetSpaceDescriptor(), TUPLE_LENGTH);
	unsigned int count = generator_VarLen->GetTuplesCount() / 2;

	cQuickDB *quickDB = new cQuickDB();
	if (!quickDB->Create(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not created!\n");
		exit(1);
	}

	// B-tree Header
	cBpTreeHeader<tKey_VarLen> *mHeader_VarLen = new cBpTreeHeader<tKey_VarLen>("btree1", BLOCK_SIZE, generator_VarLen->GetSpaceDescriptor(), tp.GetSize(generator_VarLen->GetSpaceDescriptor()), TUPLE_LENGTH / 2, variableData, DSMODE, cDStructConst::BTREE, COMPRESSION_RATIO);
	mHeader_VarLen->SetCodeType(CODETYPE);
	mHeader_VarLen->SetRuntimeMode(RUNTIME_MODE);
	mHeader_VarLen->SetHistogramEnabled(HISTOGRAMS);

	// B-tree
	cBpTree<tKey_VarLen> *mIndex_VarLen = new cBpTree<tKey_VarLen>();
	if (!mIndex_VarLen->Create(mHeader_VarLen, quickDB))
	{
		printf("TestCreate: creation failed!\n");
	}

	runtime.Start();
	for (unsigned int i = 0; i < count; i++)
	{
		if (i % 50000 == 0)
		{
			printf("Inserts: %d\r", i);
			fflush(stdout);
		}
		tKey_VarLen* mTuple = generator_VarLen->GetTuple(i);
		mIndex_VarLen->Insert(*mTuple, generator_VarLen->GetTupleData(i));
	}
	runtime.Stop();
	printf("\n");
	runtime.Print(" - Insert time\n");
	printf("Performance: %.1f Inserts/s\n\n", mIndex_VarLen->GetHeader()->GetItemCount() / runtime.GetRealTime());

	// -----------------------------------------------------------------------------
	cQueryProcStat queryStat;
	queryStat.Reset();
	cTimer queryTimer;
	queryTimer.Start();
	for (unsigned int i = 0; i < count; i++)
	{
		if (i % 50000 == 0)
		{
			printf("Queries: %d\r", i);
			fflush(stdout);
		}

		tKey_VarLen* mTuple = generator_VarLen->GetTuple(i);
		cTreeItemStream<tKey_VarLen>* resultSet = mIndex_VarLen->PointQuery(*mTuple, &queryStat);
		if (resultSet->GetItemCount() != 1)
		{
			printf("Critical Error: Item was not found!");
		}
		else
		{
			// check data
			char* resultData = (char*)(resultSet->GetItem() + tKey_VarLen::GetSize(resultSet->GetItem(), generator_VarLen->GetSpaceDescriptor()));
			char dataLength = generator_VarLen->GetTupleData(i)[0];
			char dataLength2 = resultData[0];
			/*for (unsigned int j = 1; j < dataLength; j++)
			{
			if (mData_VarLen[i][j] != resultData[j])
			{
			printf("Critical Error: Data are not correct: item order: %u,%u!", i, j);
			mKey_FixedLen[i].Print("\n", mSD_VarLen);
			}
			}*/
		}

		resultSet->CloseResultSet();
	}
	queryTimer.Stop();
	printf("\n");
	queryTimer.Print(" - Query time\n");
	printf("Performance: %.1f Queries/s\n", mIndex_VarLen->GetHeader()->GetItemCount() / queryTimer.GetRealTime());
	queryStat.PrintLAR();
	printf("\n");
	// -----------------------------------------------------------------------------

	//quickDB->GetNodeCache()->GetCacheStatistics()->Print();
	delete mIndex_VarLen;
	quickDB->Close();
	delete quickDB;
	delete mHeader_VarLen;
}

bool Open_VariableLen()
{
	bool debug = false;
	cTimer runtime;
	tKey_VarLen tp(generator_VarLen->GetSpaceDescriptor(), TUPLE_LENGTH);

	cQuickDB *quickDB = new cQuickDB();
	if (!quickDB->Open(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not opened!\n");
		exit(1);
	}

	// B-tree
	cBpTreeHeader<tKey_VarLen> *mHeader_VarLen = new cBpTreeHeader<tKey_VarLen>("btree1", BLOCK_SIZE, generator_VarLen->GetSpaceDescriptor(), tp.GetSize(generator_VarLen->GetSpaceDescriptor()), DATA_LENGTH, false, DSMODE, cDStructConst::BTREE, COMPRESSION_RATIO);
	mHeader_VarLen->SetCodeType(CODETYPE);
	mHeader_VarLen->SetRuntimeMode(RUNTIME_MODE);
	mHeader_VarLen->SetHistogramEnabled(HISTOGRAMS);

	cBpTree<tKey_VarLen> *mIndex_VarLen = new cBpTree<tKey_VarLen>();
	if (!mIndex_VarLen->Open(mHeader_VarLen, quickDB, false))
	{
		mHeader_VarLen->Print();
		printf("TestCreate: open failed!\n");
		exit(0);
	}

	runtime.Start();
	for (unsigned int i = generator_VarLen->GetTuplesCount() / 2; i < generator_VarLen->GetTuplesCount(); i++)
	{
		if (i % 50000 == 0)
		{
			printf("Inserts: %d\r", i);
			fflush(stdout);
		}
		tKey_VarLen* mTuple = generator_VarLen->GetTuple(i);
		mIndex_VarLen->Insert(*mTuple, generator_VarLen->GetTupleData(i));
	}
	runtime.Stop();
	printf("\n");
	runtime.Print(" - Insert time (after open)\n");
	tKey_VarLen* mTupleQl = generator_VarLen->GetTuple(0);
	tKey_VarLen* mTupleQh = generator_VarLen->GetTuple(1);
	cTreeItemStream<tKey_VarLen>* result = mIndex_VarLen->RangeQuery(*mTupleQl, *mTupleQh);
	int resultSize = result->GetItemCount();
	result->CloseResultSet();

	printf("Inner/Leaf Nodes: %d/%d ", mIndex_VarLen->GetHeader()->GetInnerNodeCount(), mIndex_VarLen->GetHeader()->GetLeafNodeCount());
	//quickDB->GetNodeCache()->GetCacheStatistics()->Print();
	printf("Index Size: %.2f MB\n", mIndex_VarLen->GetIndexSizeMB(BLOCK_SIZE));

	delete mIndex_VarLen;
	quickDB->Close();
	delete quickDB;
	delete mHeader_VarLen;

	// printf("Index Size: %.2f MB\n", FileSize(dbPath));

	return true;
}

void checkItem_variablelen(cBpTree<tKey_VarLen> *index, unsigned int j)
{
	tKey_VarLen* mTuple = generator_VarLen->GetTuple(j);
	cTreeItemStream<tKey_VarLen>* result = index->PointQuery(*mTuple);
	if (result->GetItemCount() != 1)
	{
		printf("Polozka %d nenalezena: ", j);
		mTuple->Print("\n", generator_VarLen->GetSpaceDescriptor());
	}
	const char* item = result->GetItem();
	const char* data = item + tKey_VarLen::GetSize(item, generator_VarLen->GetSpaceDescriptor());

	if (mTuple->Compare(item, generator_VarLen->GetSpaceDescriptor()) != 0 || !cDataVarlen::Compare(generator_VarLen->GetTupleData(j), data, cDataVarlen::GetSize(data)))
	{
		printf("Polozka %d nekoresponduje\n", j);
		mTuple->Print(" - inserted key\n", generator_VarLen->GetSpaceDescriptor());
		tKey_VarLen::Print(item, " - readed key\n", generator_VarLen->GetSpaceDescriptor());
		cDataVarlen::Print(generator_VarLen->GetTupleData(j), " - inserted data\n");
		cDataVarlen::Print(data, " - readed data\n");
	}
	result->CloseResultSet();
}

// **********************************************************************
// ********************** FixLen Items **********************************
// **********************************************************************

/**
* Perform point query on the B-tree and compare the result with the tuples stored in the mKey_Fixedlen.
*/
bool checkItem_fixedlen(cBpTree<tKey_FixedLen> *index, unsigned int j)
{
	bool ret = true;

	tKey_FixedLen *tuple = generator_FixedLen->GetTuple(j);
	cTreeItemStream<tKey_FixedLen>* result = index->PointQuery(*tuple);
	if (result->GetItemCount() != 1)
	{
		if (DSMODE == cDStructConst::DSMODE_DEFAULT)
			nofNotFoundItems++;
		else
		{
			printf("Polozka %d nenalezena: ", j);
			tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
		}
	}
	else
	{
		const char* item = result->GetItem();
		if (tuple->Compare(item, generator_FixedLen->GetSpaceDescriptor()) != 0)
		{
			printf("Polozka %d nekoresponduje\n", j);
			tuple->Print(" - inserted key\n", generator_FixedLen->GetSpaceDescriptor());
			tKey_FixedLen::Print(item, " - readed key\n", generator_FixedLen->GetSpaceDescriptor());
		}
		else
		{
			ret = true;
		}
	}
	result->CloseResultSet();
	return ret;
}

/**
* Create the B-tree and insert one half of the data.
*/
void Create_FixedLen()
{
	cTimer runtime;

	cQueryProcStat queryStat;
	cTuple tp(generator_FixedLen->GetSpaceDescriptor());
	char* resultData = new char[DATA_LENGTH];
	nofDeletedItems = 0;
	tKey_FixedLen *tuple;

	// creation of a new persistent file and opening the cache
	cQuickDB *quickDB = new cQuickDB();
	if (!quickDB->Create(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not created!\n");
		exit(1);
	}

	// B-tree
	cBpTreeHeader<tKey_FixedLen> *mHeader_FixedLen = new cBpTreeHeader<tKey_FixedLen>("btree1", BLOCK_SIZE, generator_FixedLen->GetSpaceDescriptor(), generator_FixedLen->GetSpaceDescriptor()->GetTypeSize(), DATA_LENGTH, false, DSMODE, cDStructConst::BTREE, COMPRESSION_RATIO);
	mHeader_FixedLen->SetRuntimeMode(RUNTIME_MODE);
	mHeader_FixedLen->SetCodeType(CODETYPE);
	mHeader_FixedLen->SetHistogramEnabled(HISTOGRAMS);
	mHeader_FixedLen->SetInMemCacheSize(INMEMCACHE_SIZE);

	cBpTree<tKey_FixedLen> *mIndex_FixedLen = new cBpTree<tKey_FixedLen>();
	if (!mIndex_FixedLen->Create(mHeader_FixedLen, quickDB))
	{
		printf("TestCreate: creation failed!\n");
	}

	// inserting tuples into the B-tree
	runtime.Start();
	for (unsigned int i = 0; i <= generator_FixedLen->GetTuplesCount() / 2; i++)
	{
		if (i % 50000 == 0)
		{
			printf("Inserts: %d\r", i);
			fflush(stdout);
		}

		tuple = generator_FixedLen->GetTuple(i);

		mIndex_FixedLen->Insert(*tuple, mData_FixedLen);

		/*
		for (unsigned int k = i ; k <= i ; k++)
		{
		if (k % 50000 == 0)
		{
		printf("Queries: %d\r", k);
		fflush(stdout);
		}

		cTreeItemStream<tKey_FixedLen>* resultSet = mIndex_FixedLen->PointQuery(mKey_FixedLen[k]);
		if (resultSet->GetItemCount() != 1)
		{
		printf("Critical Error: Item was not found!");
		mKey_FixedLen[k].Print("\n", mSD_FixedLen);
		}
		else
		{
		// check data
		char* resultData = (char*)(resultSet->GetItem() + mIndex_FixedLen->GetHeader()->GetKeySize());
		for (unsigned int j = 0 ; j < DATA_LENGTH ; j++)
		{
		if (mData_FixedLen[j] != resultData[j])
		{
		printf("Critical Error: Data are not correct: item order: %u,%u!", k, j);
		mKey_FixedLen[k].Print("\n", mSD_FixedLen);
		}
		}
		}
		resultSet->CloseResultSet();
		}*/
	}
	runtime.Stop();
	printf("\n");
	runtime.Print(" - Insert time\n");
	printf("Performance: %.1f Inserts/s\n\n", mIndex_FixedLen->GetHeader()->GetItemCount() / runtime.GetRealTime());

	// mIndex_FixedLen->PrintInfo();
	// quickDB->GetNodeCache()->GetCacheStatistics()->Print();
	if (HISTOGRAMS)
	{
		mIndex_FixedLen->PrintDimDistribution();
	}

	bool insertOnly = false;

	queryStat.Reset();
	printf("QUERY PROCESSING WITHOUT RESULT SET\n");
	if (!insertOnly)
	{
		// Perform queries
		cTimer querytime;
		querytime.Start();
		for (unsigned int i = 0; i <= generator_FixedLen->GetTuplesCount() / 2; i++)
		{
			if (i % 50000 == 0)
			{
				printf("Queries: %d\r", i);
				fflush(stdout);
			}

			tuple = generator_FixedLen->GetTuple(i);
			if (!mIndex_FixedLen->PointQuery(*tuple, resultData, &queryStat))
			{
				printf("Critical Error: Item was not found!");
				tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
			}
			else
			{
				// check data
				for (unsigned int j = 0; j < DATA_LENGTH; j++)
				{
					if (mData_FixedLen[j] != resultData[j])
					{
						printf("Critical Error: Data are not correct: item order: %u,%u!", i, j);
						tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
					}
				}
			}
		}
		querytime.Stop();
		printf("\n");
		querytime.Print(" - Query time\n");
		printf("Performance: %.1f Queries/s\n", mIndex_FixedLen->GetHeader()->GetItemCount() / querytime.GetRealTime());
		queryStat.PrintLAR();
		printf("\n");
		// ----------------------------------------------------
	}

	queryStat.Reset();
	printf("QUERY PROCESSING WITH RESULT SET\n");
	if (!insertOnly)
	{
		// Perform queries
		cTimer querytime;
		querytime.Start();
		for (unsigned int i = 0; i <= generator_FixedLen->GetTuplesCount() / 2; i++)
		{
			if (i % 50000 == 0)
			{
				printf("Queries: %d\r", i);
				fflush(stdout);
			}

			tuple = generator_FixedLen->GetTuple(i);

			cTreeItemStream<tKey_FixedLen>* resultSet = mIndex_FixedLen->PointQuery(*tuple, &queryStat);

			if (resultSet->GetItemCount() != 1)
			{
				printf("Critical Error: Item was not found!");
				tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
			}
			else
			{
				// check data
				char* resultData2 = (char*)(resultSet->GetItem() + mIndex_FixedLen->GetHeader()->GetKeySize());
				for (unsigned int j = 0; j < DATA_LENGTH; j++)
				{
					if (mData_FixedLen[j] != resultData2[j])
					{
						printf("Critical Error: Data are not correct: item order: %u,%u!", i, j);
						tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
					}
				}
			}
			resultSet->CloseResultSet();

			bool ret = false;
			if (i % 10000 == 0)
			{
				if (DSMODE == cDStructConst::DSMODE_DEFAULT)
				{
					mIndex_FixedLen->Delete(*tuple);
					ret = mIndex_FixedLen->PointQuery(*tuple, resultData);
					if (ret)
					{
						printf("Critical Error: Deleted tuple is found!\n");
					}
					else
					{
						nofDeletedItems++;
					}
				}
			}

		}
		querytime.Stop();
		printf("\n");
		querytime.Print(" - Query time\n");
		printf("Performance: %.1f Queries/s\n", mIndex_FixedLen->GetHeader()->GetItemCount() / querytime.GetRealTime());
		queryStat.PrintLAR();
		printf("\n");
		// ----------------------------------------------------
	}

	delete mIndex_FixedLen;
	quickDB->Close();
	delete quickDB;
	delete mHeader_FixedLen;
	delete resultData;
}

/**
* Open the B-tree and insert the second half of the data.
*/
bool Open_FixedLen()
{
	cQueryProcStat queryStat;
	cTimer runtime;
	cTuple tuple(generator_FixedLen->GetSpaceDescriptor());
	nofNotFoundItems = 0;

	// opening of an existing persistent file and opening the cache
	cQuickDB *quickDB = new cQuickDB();
	if (!quickDB->Open(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not opened!\n");
		exit(1);
	}

	// B-tree
	cBpTreeHeader<tKey_FixedLen> *mHeader_FixedLen = new cBpTreeHeader<tKey_FixedLen>("btree1", BLOCK_SIZE, generator_FixedLen->GetSpaceDescriptor(), tuple.GetSize(generator_FixedLen->GetSpaceDescriptor()), sizeof(int), false, DSMODE, cDStructConst::BTREE, COMPRESSION_RATIO);
	mHeader_FixedLen->SetCodeType(CODETYPE);
	mHeader_FixedLen->SetRuntimeMode(RUNTIME_MODE);
	mHeader_FixedLen->SetHistogramEnabled(HISTOGRAMS);
	mHeader_FixedLen->SetInMemCacheSize(INMEMCACHE_SIZE);

	cBpTree<tKey_FixedLen> *mIndex_FixedLen = new cBpTree<tKey_FixedLen>();
	if (!mIndex_FixedLen->Open(mHeader_FixedLen, quickDB, false))
	{
		printf("TestCreate: opening failed!\n");
		exit(0);
	}

	runtime.Start();
	for (unsigned int i = generator_FixedLen->GetTuplesCount() / 2 + 1; i < ITEM_COUNT; i++)
	{
		if (i % 50000 == 0)
		{
			printf("Inserts: %d\r", i);
			fflush(stdout);
		}
		tuple = *generator_FixedLen->GetTuple(i);
		mIndex_FixedLen->Insert(tuple, mData_FixedLen);
	}
	runtime.Stop();
	printf("\n");
	runtime.Print(" - Insert time (after open)\n");

	cTreeItemStream<tKey_FixedLen>* result = mIndex_FixedLen->RangeQuery(*mAuxTuple1, *mAuxTuple2);
	if (result->GetItemCount() != 0)
	{
		printf("open_fixedlen - empty range query failed (it returns some result)\n");
	}
	result->CloseResultSet();

	if (MULTIPLERANGEQUERY)
	{
		cRangeQueryConfig config;
		config.SetQueryProcessingType(cRangeQueryProcessorConstants::RQ_BTREE_SEQ);
		config.SetLeafIndicesCapacity(1024);
		config.SetMaxReadNodes(1024);
		config.SetFinalResultSize(0);

		for (unsigned int i = 0; i < generator_FixedLen->GetTuplesCount(); i += QUERIES_IN_MULTIQUERY)
		{
			if (i % (1000 * QUERIES_IN_MULTIQUERY) == 0)
			{
				printf("#Query: %d      \r", i);
			}

			unsigned int queriesCount = ((ITEM_COUNT - i >= QUERIES_IN_MULTIQUERY) ? QUERIES_IN_MULTIQUERY : ITEM_COUNT - i);

			tKey_FixedLen *qls = new tKey_FixedLen[queriesCount];
			tKey_FixedLen *qhs = new tKey_FixedLen[queriesCount];

			for (unsigned int j = 0; j < queriesCount; j++)
			{				
				qls[j] = *generator_FixedLen->GetTuple(i + j);
				qhs[j] = *generator_FixedLen->GetTuple(i + j);
			}			

			cTreeItemStream<tKey_FixedLen>* result = mIndex_FixedLen->BatchRangeQuery(qls, qhs, &config, queriesCount, NULL, NULL);

			if (result->GetItemCount() == 0)
			{
				printf("\nCritical Error: Items was not found: (%i - %i!)\n", i, i + QUERIES_IN_MULTIQUERY);
			}
			result->CloseResultSet();
		}
	}
	else
	{
		// test where we search for every tuple in the B-tree
		for (unsigned int j = 0; j < ITEM_COUNT; j++)
		{
			if (j % 50000 == 0)
			{
				printf("checkItem: %d\r", j);
				fflush(stdout);
			}

			if (!checkItem_fixedlen(mIndex_FixedLen, j))
			{
				delete mIndex_FixedLen;
				quickDB->Close();
				delete quickDB;
				delete mHeader_FixedLen;

				return false;
			}
		}
	}

	if (nofDeletedItems == nofNotFoundItems)
	{
		printf("Number of successfully deleted items is : %d\n", nofDeletedItems);
	}
	else
	{
		printf("Number of deleted items != Number of not found items !!!\n");
	}

	//quickDB->GetNodeCache()->GetCacheStatistics()->Print();
	printf("Inner/Leaf Nodes: %d/%d ", mIndex_FixedLen->GetHeader()->GetInnerNodeCount(), mIndex_FixedLen->GetHeader()->GetLeafNodeCount());
	printf("Index Size: %.2f MB\n", mIndex_FixedLen->GetIndexSizeMB(BLOCK_SIZE));

	if ((DSMODE == cDStructConst::DSMODE_RI) && (RUNTIME_MODE == cDStructConst::RTMODE_VALIDATION))
	{
		mIndex_FixedLen->PrintSubNodesDistribution("snDistribution.txt");
	}

	delete mIndex_FixedLen;
	quickDB->Close();
	delete quickDB;
	delete mHeader_FixedLen;

	return true;
}

// **********************************************************************
// ********************** Bulkloading **********************************
// **********************************************************************

void Bulkloading_FixedLen()
{
	InitializeTuplesGenerator(cDStructConst::TUPLE);

	cQueryProcStat queryStat;
	char* resultData = new char[DATA_LENGTH];
	
	cQuickDB *quickDB = new cQuickDB();
	if (!quickDB->Create(dbPath, CACHE_SIZE, MAX_NODE_INMEM_SIZE, BLOCK_SIZE))
	{
		printf("Critical Error: Cache Data File was not created!\n");
		exit(1);
	}

	printf("Fixed length B-tree Bulkload test (tuple length/data length: %d/%d)\n", TUPLE_LENGTH, DATA_LENGTH);


	cBpTreeHeader<TKey> *mHeader = new cBpTreeHeader<TKey>("btree1", BLOCK_SIZE, generator_FixedLen->GetSpaceDescriptor(), generator_FixedLen->GetSpaceDescriptor()->GetTypeSize(), DATA_LENGTH, false, cDStructConst::DSMODE_DEFAULT, cDStructConst::BTREE, COMPRESSION_RATIO);
	mHeader->SetRuntimeMode(RUNTIME_MODE);
	mHeader->SetHistogramEnabled(HISTOGRAMS);
	mHeader->SetInMemCacheSize(INMEMCACHE_SIZE);

	BpTree *tree = new BpTree();
	if (!tree->Create(mHeader, quickDB))
	{
		printf("Critical Error: Creation of Index file failed!\n");
		exit(1);
	}

	tBulkload *bulkLoad = new tBulkload(mHeader, tree, 0.8, 1.0, ITEM_COUNT, cSortType::Lexicographical, DATA_LENGTH, generator_FixedLen->GetSpaceDescriptor());

	cTimer runtime;
	runtime.Start();
	for (uint i = 0; i < generator_FixedLen->GetTuplesCount(); i++)
	{
		bulkLoad->Add(*generator_FixedLen->GetNextTuple(), mData_FixedLen);
	}

	bulkLoad->Sort();
	bulkLoad->CreateBpTree();
	runtime.Stop();
	printf("\n");
	runtime.Print(" - Sort time\n");
	printf("Inner/Leaf Nodes: %d/%d \n", tree->GetHeader()->GetInnerNodeCount(), tree->GetHeader()->GetLeafNodeCount());
	printf("Index Size: %.2f MB\n", tree->GetIndexSizeMB(BLOCK_SIZE));



	queryStat.Reset();
	// Perform queries
	cTimer querytime;
	querytime.Start();
	for (unsigned int i = 0; i < generator_FixedLen->GetTuplesCount(); i++)
	{
		if (i % 50000 == 0)
		{
			printf("Queries: %d\r", i);
			fflush(stdout);
		}

		tKey_FixedLen* tuple = generator_FixedLen->GetTuple(i);
		if (!tree->PointQuery(*tuple, resultData, &queryStat))
		{
			printf("Critical Error: Item was not found!");
			tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
		}
		else
		{
			// check data
			for (unsigned int j = 0; j < DATA_LENGTH; j++)
			{
				if (mData_FixedLen[j] != resultData[j])
				{
					printf("Critical Error: Data are not correct: item order: %u,%u!", i, j);
					tuple->Print("\n", generator_FixedLen->GetSpaceDescriptor());
				}
			}
		}
	}
	querytime.Stop();
	printf("\n");
	querytime.Print(" - Query time\n");
	printf("Performance: %.1f Queries/s\n", tree->GetHeader()->GetItemCount() / querytime.GetRealTime());
	queryStat.PrintLAR();
	printf("\n");

	CloseTuplesGenerator(cDStructConst::TUPLE);
}

// **********************************************************************
// ********************** Header Generator *************************
// **********************************************************************

void printHeader()
{
	//printf("---------------------------------------------\n");
	if (ITEM_TYPE == cDStructConst::TUPLE)
		printf("Fixed length B-tree test (tuple length/data length: %d/%d)\n", TUPLE_LENGTH, DATA_LENGTH);
	else
		printf("Variable length B-tree test (tuple length/data length: %d/%d)\n", TUPLE_LENGTH, DATA_LENGTH);

	switch (DSMODE)
	{
	case cDStructConst::DSMODE_DEFAULT: printf("DSMODE: DEFAULT "); break;
	case cDStructConst::DSMODE_RI: printf("DSMODE: REFERENCE ITEMS "); break;
	case cDStructConst::DSMODE_CODING: printf("DSMODE: CODING "); break;
	case cDStructConst::DSMODE_RICODING: printf("DSMODE: REFERENCE ITEMS & CODING "); break;
	}

	if ((DSMODE == cDStructConst::DSMODE_CODING) || (DSMODE == cDStructConst::DSMODE_RICODING))
	{
		printf("/ ");
		switch (CODETYPE)
		{
		case ELIAS_DELTA: printf("CODETYPE: ELIAS_DELTA "); break;
		case FIBONACCI2: printf("CODETYPE: FIBONACCI2 "); break;
		case FIBONACCI3: printf("CODETYPE: FIBONACCI3 "); break;
		case ELIAS_FIBONACCI: printf("CODETYPE: ELIAS_FIBONACCI "); break;
		case ELIAS_DELTA_FAST: printf("CODETYPE: ELIAS_DELTA_FAST "); break;
		case FIBONACCI2_FAST: printf("CODETYPE: FIBONACCI2_FAST "); break;
		case FIBONACCI3_FAST: printf("CODETYPE: FIBONACCI3_FAST "); break;
		case ELIAS_FIBONACCI_FAST: printf("CODETYPE: ELIAS_FIBONACCI_FAST "); break;
		case FIXED_LENGTH_CODING: printf("CODETYPE: FIXED_LENGTH_CODING "); break;
		case FIXED_LENGTH_CODING_ALIGNED: printf("CODETYPE: FIXED_LENGTH_CODING_ALIGNED "); break;
		}
	}
	printf("\n");
}

void InitializeTuplesGenerator(uint type)
{

	if (type == cDStructConst::NTUPLE)
	{
		if (COLLECTION == cCollection::RANDOM)
		{
			generator_VarLen = new cTuplesGenerator<cUInt, tKey_VarLen>(ITEM_COUNT, TUPLE_LENGTH, DOMAIN_MAX);
		}
		else
		{
			generator_VarLen = new cTuplesGenerator<cUInt, tKey_VarLen>(cDataCollection::COLLECTION_FILE(COLLECTION, COMPUTER), false);
		}
	}
	else
	{
		if (COLLECTION == cCollection::RANDOM)
		{
			generator_FixedLen = new cTuplesGenerator<cUInt, tKey_FixedLen>(ITEM_COUNT, TUPLE_LENGTH, DOMAIN_MAX);
		}
		else
		{
			generator_FixedLen = new cTuplesGenerator<cUInt, tKey_FixedLen>(cDataCollection::COLLECTION_FILE(COLLECTION, COMPUTER), false);
		}

		mData_FixedLen = new char[DATA_LENGTH];
		for (unsigned int j = 0; j < DATA_LENGTH; j++)
		{
			mData_FixedLen[j] = (char)j;
		}

		mAuxTuple1 = new tKey_FixedLen(generator_FixedLen->GetSpaceDescriptor()); // auxiliary tuple used during test
		mAuxTuple2 = new tKey_FixedLen(generator_FixedLen->GetSpaceDescriptor()); // auxiliary tuple used during test

		for (unsigned int j = 0; j < TUPLE_LENGTH; j++)
		{
			mAuxTuple1->SetValue(j, 150000, generator_FixedLen->GetSpaceDescriptor());
			mAuxTuple2->SetValue(j, 150000, generator_FixedLen->GetSpaceDescriptor());
		}
	}
}
void CloseTuplesGenerator(uint type)
{
	if (type == cDStructConst::NTUPLE)
	{
		generator_VarLen = NULL;
		delete generator_VarLen;
	}
	else
	{
		generator_FixedLen = NULL;
		delete generator_FixedLen;
	}
}