/*
 *   SniffJoke is a software able to confuse the Internet traffic analysis,
 *   developed with the aim to improve digital privacy in communications and
 *   to show and test some securiy weakness in traffic analysis software.
 *   
 *   Copyright (C) 2010 vecna <vecna@delirandom.net>
 *                      evilaliv3 <giovanni.pellerano@evilaliv3.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SJ_TCPTRACK_H
#define SJ_TCPTRACK_H

#include "UserConf.h"
#include "Packet.h"
#include "PacketQueue.h"
#include "SessionTrack.h"
#include "TTLfocus.h"

#include <vector>
using namespace std;

class HackPacketPoolElem {
public:
	bool *config;
	bool enabled;
	HackPacket *dummy;

	HackPacketPoolElem(bool*, HackPacket*);
};

struct PluginTrack {
	void *pluginHandler;	
	constructor_f fp_CreateHackObj;
	destructor_f fp_DeleteHackObj;
	const char *pluginpath;
};

class HackPacketPool : public vector<HackPacketPoolElem> {
private:
	/* TODO - serve un vettore che tenga traccia dei nomi dei plugin, degli stream aperti dei plugin e delle funzioni distruttici.
	 * in questo modo, quando viene chiamato ~TCPTrack, viene invicata la lista di distruttori importanti da dlsym
	 */
//	vector<PluginTrack> listOfHacks;
	struct PluginTrack listOfHacks[20];
	int n_plugin; // TEMP - DOPO SARA' VECTOR o COS'ALTRO 

	void ImportPluginList(struct sj_config *);
public:
	HackPacketPool(struct sj_config *);
};

class TCPTrack {
private:
	struct sj_config *runcopy;
	bool youngpacketspresent;
	struct timespec clock;
	PacketQueue p_queue;
	SessionTrackMap sex_map;
	TTLFocusMap ttlfocus_map;
	HackPacketPool hack_pool;

	bool check_evil_packet(const unsigned char*, unsigned int);
	bool check_uncommon_tcpopt(const struct tcphdr*);
	bool percentage(float, unsigned int);
	float logarithm(int);

	SessionTrack* init_sessiontrack(const Packet&);
	void clear_session(SessionTrackMap::iterator stm_it);

	TTLFocus* init_ttlfocus(unsigned int);
	void enque_ttl_probe(const Packet&, TTLFocus&);
	bool analyze_ttl_stats(TTLFocus&);

	void analyze_incoming_ttl(Packet&);
	Packet* analyze_incoming_icmp(Packet&);
	Packet* analyze_incoming_synack(Packet&);
	Packet* analyze_incoming_rstfin(Packet&);
	
	void manage_outgoing_packets(Packet&);
	void mark_real_syn_packets_SEND(unsigned int);
	
	void inject_hack_in_queue(Packet&, const SessionTrack*);
	void last_pkt_fix(Packet&);

public:
	TCPTrack(UserConf*);
	~TCPTrack(void);

	bool writepacket(const source_t, const unsigned char*, int);
	Packet* readpacket(void);
	void analyze_packets_queue(void);
	void force_send(void);
};

#endif /* SJ_TCPTRACK_H */
