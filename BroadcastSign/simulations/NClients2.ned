package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;


network NClients2
{
    parameters:
        int n;
        //int k = 0;
    types:
        channel fiberline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 512Mbps;
            thruputDisplayFormat = "u";
        }
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 0.1us;
            datarate = 1Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
        cli[n]: StandardHost {
            parameters:
                @display("i=device/laptop"); //;p=250,100,col,100");
        }
    connections:
//        for i=0..int((n/k)-1) {
//            cli[i].pppg++ <--> ethernetline <--> r[0].pppg++;
//        }
//        for i=int(n/5)..int((2*n/5)-1) {
//            cli[i].pppg++ <--> ethernetline <--> r[1].pppg++;
//        }
//        for i=int(2*n/5)..int((3*n/5)-1) {
//            cli[i].pppg++ <--> ethernetline <--> r[2].pppg++;
//        }
//        for i=int(3*n/5)..int((4*n/5)-1) {
//            cli[i].pppg++ <--> ethernetline <--> r[3].pppg++;
//        }
//        for i=int(4*n/5)..(n-1) {
//            cli[i].pppg++ <--> ethernetline <--> r[4].pppg++;
//        }
        
        for i=0..(n-1), for j=(i+1)..(min(n-1, i+3)) {
	        cli[i].pppg++ <--> fiberline <--> cli[j].pppg++;
        }
        cli[n-2].pppg++ <--> fiberline <--> cli[0].pppg++;
        cli[0].pppg++ <--> fiberline <--> cli[n-1].pppg++;
        cli[1].pppg++ <--> fiberline <--> cli[n-1].pppg++;
        
//        r[0].pppg++ <--> fiberline <--> r[1].pppg++;
//        r[0].pppg++ <--> fiberline <--> r[2].pppg++;
//        r[0].pppg++ <--> fiberline <--> r[3].pppg++;
//        r[0].pppg++ <--> fiberline <--> r[4].pppg++;
//        
//        r[1].pppg++ <--> fiberline <--> r[2].pppg++;
//        r[1].pppg++ <--> fiberline <--> r[3].pppg++;
//        r[1].pppg++ <--> fiberline <--> r[4].pppg++;
//        
//        r[2].pppg++ <--> fiberline <--> r[3].pppg++;
//        r[2].pppg++ <--> fiberline <--> r[4].pppg++;
//
//		r[3].pppg++ <--> fiberline <--> r[4].pppg++;
}
