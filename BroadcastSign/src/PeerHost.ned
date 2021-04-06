package broadcastsign;

import inet.applications.contract.IApp;

//
// Video streaming client.
//
// This module sends one "video streaming request" to serverAddress:serverPort at time startTime
// and receives stream from ~UdpVideoStreamServer server.
//
// Statistics:
//
// packetReceived: statistic of received packets and received bytes.
// endToEndDelay: statistic of delays of incoming packets.
//
// @see ~UdpVideoStreamServer
//
simple PeerHost like IApp {
    parameters:
        @class(inet::Peer);
        int localPort = default(-1);  // local port (-1: use ephemeral port)

        double startTimePar @unit("s") = default(0s);
        int nPar; 
        int fPar;
        int kPar;
        double msgSizePar; // in bits
        string byzStringPar;
        string outputPrefixPar  = default("outputfile");
        
        bool OPTIM_SENDTOECHO = default(false); 
        bool OPTIM_ECHOTOECHO = default(false);
		bool OPTIM_ECHOTOREADY = default(false);
		bool OPTIM_CUTECHOREADY_AFTER_DELIVER = default(false);
		bool OPTIM_CUT_ECHO_AFTER_READY_RECEIVED = default(false);
		bool OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED = default(false);
		bool OPTIM_CUT_ECHO_TO_READY_NEIGHBORS = default(false);
		bool OPTIM_BRACHA_OVERPROVISIONING = default(false);
		bool OPTIM_BITSET = default(false);
		bool OPTIM_MESSAGE_FIELDS = default(false); 
		bool OPTIM_PAYLOAD_AVOID_IF_RECEIVED = default(false);
		bool OPTIM_SEND_2f1 = default(false);
		bool ASYNC = default(false);
		
		int seed = default(12345);
        
        @display("i=block/app");
        //@lifecycleSupport;
        //double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        //double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation
        //@signal[packetReceived](type=inet::Packet);
        //@statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        //@statistic[throughput](title="throughput"; unit=bps; source="throughput(packetReceived)"; record=vector);
        //@statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,vector; interpolationmode=none);
        
	gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
}

