#ifndef REPLICATEDLOG_H
#define REPLICATEDLOG_H

#include "System/Containers/List.h"
#include "System/Events/Callable.h"
#include "Framework/Transport/TransportReader.h"
#include "Framework/Transport/TransportWriter.h"
#include "Framework/Paxos/PaxosProposer.h"
#include "Framework/Paxos/PaxosAcceptor.h"
#include "Framework/Paxos/PaxosLearner.h"
#include "Framework/PaxosLease/PaxosLease.h"
#include "Framework/ReplicatedLog/ReplicatedDB.h"
#include "ReplicatedLogMsg.h"
#include "LogCache.h"
#include "LogQueue.h"

#define CATCHUP_TIMEOUT	5000
#define USE_TCP			1


class ReplicatedLog
{
public:
	ReplicatedLog();

	static ReplicatedLog*		Get();
	bool						Init();
	void						OnRead();
	bool						Append(ByteString &value);
	void						SetReplicatedDB(ReplicatedDB* replicatedDB_);
	Transaction*				GetTransaction();
	bool						GetLogItem(uint64_t paxosID, ByteString& value);
	uint64_t					GetPaxosID();
	void						SetPaxosID(Transaction* transaction, uint64_t paxosID);
	bool						IsMaster();
	int							GetMaster();
	unsigned 					GetNodeID();
	void						StopPaxos();
	void						StopMasterLease();
	void						ContinuePaxos();
	void						ContinueMasterLease();
	bool						IsAppending();
	bool						IsSafeDB();
	void						OnPaxosLeaseMsg(uint64_t paxosID, unsigned nodeID);

private:
	void						InitTransport();
	void						ProcessMsg();
	void						OnPrepareRequest();
	void						OnPrepareResponse();
	void						OnProposeRequest();
	void						OnProposeResponse();
	void						OnLearnChosen();
	void						OnRequestChosen();
	void						OnRequest();
	void						OnCatchupTimeout();
	void						OnLearnLease();
	void						OnLeaseTimeout();
	void						NewPaxosRound();

	TransportReader*			reader;
	TransportWriter**			writers;
	MFunc<ReplicatedLog>		onRead;
	PaxosProposer				proposer;
	PaxosAcceptor				acceptor;
	PaxosLearner				learner;
	PaxosLease					masterLease;
	PaxosMsg					pmsg;
	ReplicatedLogMsg			rmsg;
	ByteBuffer					value;
	uint64_t					highestPaxosID;
	LogCache					logCache;
	LogQueue					logQueue;
	MFunc<ReplicatedLog>		onCatchupTimeout;
	CdownTimer					catchupTimeout;
	MFunc<ReplicatedLog>		onLearnLease;
	MFunc<ReplicatedLog>		onLeaseTimeout;
	ReplicatedDB*				replicatedDB;
	bool						safeDB;
};
#endif
