package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;


network NClientsRandom
{
    parameters:
        int n = 20;
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
        //r[k]: Router {}
        cli[n]: StandardHost {
            parameters:
                @display("i=device/laptop"); //;p=250,100,col,100");
        }
    connections:
        cli[0].pppg++ <--> fiberline <--> cli[4].pppg++;
        cli[0].pppg++ <--> fiberline <--> cli[7].pppg++;
        cli[0].pppg++ <--> fiberline <--> cli[15].pppg++;
        cli[1].pppg++ <--> fiberline <--> cli[7].pppg++;
        cli[1].pppg++ <--> fiberline <--> cli[13].pppg++;
        cli[1].pppg++ <--> fiberline <--> cli[16].pppg++;
        cli[2].pppg++ <--> fiberline <--> cli[11].pppg++;
        cli[2].pppg++ <--> fiberline <--> cli[12].pppg++;
        cli[2].pppg++ <--> fiberline <--> cli[16].pppg++;
        cli[3].pppg++ <--> fiberline <--> cli[9].pppg++;
        cli[3].pppg++ <--> fiberline <--> cli[12].pppg++;
        cli[3].pppg++ <--> fiberline <--> cli[18].pppg++;
        cli[4].pppg++ <--> fiberline <--> cli[15].pppg++;
        cli[4].pppg++ <--> fiberline <--> cli[19].pppg++;
        cli[5].pppg++ <--> fiberline <--> cli[6].pppg++;
        cli[5].pppg++ <--> fiberline <--> cli[18].pppg++;
        cli[5].pppg++ <--> fiberline <--> cli[19].pppg++;
        cli[6].pppg++ <--> fiberline <--> cli[9].pppg++;
        cli[6].pppg++ <--> fiberline <--> cli[14].pppg++;
        cli[7].pppg++ <--> fiberline <--> cli[18].pppg++;
        cli[8].pppg++ <--> fiberline <--> cli[10].pppg++;
        cli[8].pppg++ <--> fiberline <--> cli[13].pppg++;
        cli[8].pppg++ <--> fiberline <--> cli[14].pppg++;
        cli[9].pppg++ <--> fiberline <--> cli[17].pppg++;
        cli[10].pppg++ <--> fiberline <--> cli[11].pppg++;
        cli[10].pppg++ <--> fiberline <--> cli[17].pppg++;
        cli[11].pppg++ <--> fiberline <--> cli[14].pppg++;
        cli[12].pppg++ <--> fiberline <--> cli[19].pppg++;
        cli[13].pppg++ <--> fiberline <--> cli[15].pppg++;
        cli[16].pppg++ <--> fiberline <--> cli[17].pppg++;
}
