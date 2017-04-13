# Kubos High Level Architecture

@dot
digraph arch {
    node [shape=box];
    compound = true;
    rankdir=LR;

    client1 [label="Client (shell, GS)"];

    subgraph cluster0 {
        label = "Kubos";

        subgraph cluster1 {
            label = "Command & Control";
            cc1 [label="Command Service"];
            cc2 [label="Command Registry"];
            cc3 [label="Service1"];
            cc4 [label="Service2"];
            cc5 [label="Service3"];

            cc1 -> cc2;
            
            cc2 -> cc3;
            cc2 -> cc4;
            cc2 -> cc5;
        }

        subgraph cluster2 {
            label = "Scheduler Service";
            job1 [label="Job"];
            job2 [label="Job"];
            job3 [label="Job"];
        }

        subgraph cluster3 {
            label = "Telemetry";
            
            pub1 [label="Publisher"];
            pub2 [label="Publisher"];
            pub3 [label="Publisher"];

            sub1 [label="Subscriber"];
            sub2 [label="Subscriber"];
            sub3 [label="Subscriber"];

            pub1 -> sub1;
            pub2 -> sub2;
            pub3 -> sub3;
        }

        subgraph cluster4 {
            label = "Synchronous APIs";

            api1 [label="Application"];
            api2 [label="Core Kubos APIs"];
        }

        updater [label="Software Updater"];
    }

    client1 -> cc1 [lhead=cluster1];
    cc1 -> client1 [ltail=cluster1];

    cc4:e -> job2:e [lhead=cluster2, ltail=cluster1];
    cc4:e -> pub2:w [lhead=cluster3, ltail=cluster1];
    cc4:e -> api1:w [lhead=cluster4, ltail=cluster1];
    cc4:e -> updater:w [ltail=cluster1];
}
@enddot