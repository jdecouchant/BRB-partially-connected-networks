package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;


network fullyconnected
{
    parameters:
        int n;
        
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
        //r[k]: Router {}
        cli[n]: StandardHost {
            parameters:
                @display("i=device/laptop"); //;p=250,100,col,100");
        }
    connections:
        
        for i=0..(n-1), for j=(i+1)..(n-1) {
	        cli[i].pppg++ <--> fiberline <--> cli[j].pppg++;
        }
}
