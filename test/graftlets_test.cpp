#include <gtest/gtest.h>
#include "fixture.h"
#include "server.h"
#include "test.h"
#define INCLUDE_DEPENDENCY_GRAPH
#include "GraftletLoader.h"

TEST(DependencyGraph, dependencies)
{
    using Graph = graftlet::GraftletLoader::DependencyGraph;
    Graph graph;
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {};

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.empty(), true);
        graph.findCycles();
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B:1.0"},
            {"B",GRAFTLET_MKVER(1,0), "C:1.0"},
            {"C",GRAFTLET_MKVER(1,0), ""},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.empty(), true);
        graph.findCycles();
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), " B : 1 . 2 , C: 3, D "},
            {"B",GRAFTLET_MKVER(1,0), "C:4.5, D: 7"},
            {"C",GRAFTLET_MKVER(1,0), "E : 3.3"},
            {"D",GRAFTLET_MKVER(1,0), ""},
        };

        graph.initialize(vec);
        {
            Graph::list_t* list = &graph.m_graph["A"];
            EXPECT_EQ(list->size(), 3);
            auto it = list->begin();
            EXPECT_EQ(it->first,"B");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(1,2));
            ++it;
            EXPECT_EQ(it->first,"C");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(3,0));
            ++it;
            EXPECT_EQ(it->first,"D");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(0,0));

            list = &graph.m_graph["B"];
            EXPECT_EQ(list->size(), 2);
            it = list->begin();
            EXPECT_EQ(it->first,"C");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(4,5));
            ++it;
            EXPECT_EQ(it->first,"D");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(7,0));

            list = &graph.m_graph["C"];
            EXPECT_EQ(list->size(), 1);
            it = list->begin();
            EXPECT_EQ(it->first,"E");
            EXPECT_EQ(it->second,GRAFTLET_MKVER(3,3));

            list = &graph.m_graph["D"];
            EXPECT_EQ(list->size(), 0);
        }
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 3);
        graph.findCycles();
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B:1.2, C:3, D"},
            {"B",GRAFTLET_MKVER(1,2), "C:0.5, D:7"},
            {"C",GRAFTLET_MKVER(3,1), "D:3.3"},
            {"D",GRAFTLET_MKVER(7,0), ""},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 0);
        graph.findCycles();
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B:1.2, C:3, D"},
            {"B",GRAFTLET_MKVER(1,2), "C:0.5, D:7"},
            {"C",GRAFTLET_MKVER(3,1), "D:3.3"},
            {"D",GRAFTLET_MKVER(3,3), ""},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 2);
        graph.findCycles();
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B, C, D"},
            {"B",GRAFTLET_MKVER(1,0), "C, D"},
            {"C",GRAFTLET_MKVER(1,0), "D:, B"}, //invalid format
            {"D",GRAFTLET_MKVER(1,0), ""},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 3);
        std::string s = graph.findCycles(true);
        EXPECT_TRUE(s.empty());
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B, C, D"},
            {"B",GRAFTLET_MKVER(1,0), "C, D"},
            {"C",GRAFTLET_MKVER(1,0), "D, B"},
            {"D",GRAFTLET_MKVER(1,0), ""},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 0);
        std::string s = graph.findCycles(true);
        EXPECT_FALSE(s.empty());
        std::cout << s << "\n";
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "B, C, D"},
            {"B",GRAFTLET_MKVER(1,0), "C, D"},
            {"C",GRAFTLET_MKVER(1,0), "D"},
            {"D",GRAFTLET_MKVER(1,0), "A"},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 0);
        std::string s = graph.findCycles(true);
        EXPECT_FALSE(s.empty());
        std::cout << s << "\n";
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "D"},
            {"B",GRAFTLET_MKVER(1,0), "B"},
            {"C",GRAFTLET_MKVER(1,0), "B"},
            {"D",GRAFTLET_MKVER(1,0), "C"},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 0);
        std::string s = graph.findCycles(true);
        EXPECT_FALSE(s.empty());
        std::cout << s << "\n";
    }
    {
        std::vector<std::tuple<Graph::DllName,Graph::Version,Graph::Dependencies>> vec {
            {"A",GRAFTLET_MKVER(1,0), "C, D"},
            {"B",GRAFTLET_MKVER(1,0), ""},
            {"C",GRAFTLET_MKVER(1,0), "B"},
            {"D",GRAFTLET_MKVER(1,0), "B, C, A"},
        };

        graph.initialize(vec);
        std::vector<Graph::DllName> res = graph.removeFailedDependants();
        EXPECT_EQ(res.size(), 0);
        std::string s = graph.findCycles(true);
        EXPECT_FALSE(s.empty());
        std::cout << s << "\n";
    }
}

TEST(Graftlets, calls)
{
    graftlet::GraftletLoader loader;

    loader.findGraftletsInDirectory("./", "so");
    loader.findGraftletsInDirectory("./graftlets", "so");

    graftlet::GraftletHandler plugin = loader.buildAndResolveGraftlet("myGraftlet");

    try
    {//testInt1, testInt2
        int res = plugin.invoke<int (int a)>("testGL.testInt1", 5);
        EXPECT_EQ(res, 5);
        res = plugin.invoke<int (int a)>("testGL.testInt1", 7);
        EXPECT_EQ(res, 7);

        int a = 7;
        res = plugin.invoke<int (int&&, int, int&)>("testGL.testInt2", 3, 5, a);
        EXPECT_EQ(a, 3 + 5);
        EXPECT_EQ(res, a + 3 + 5);
        res = plugin.invoke<int (int&& a, int b, int& c)>("testGL.testInt2", 7, 11, a);
        EXPECT_EQ(a, 7 + 11);
        EXPECT_EQ(res, a + 7 + 11);
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << "\n";
        EXPECT_EQ(true,false);
    }

    try
    {//testString1
        std::string a = "aaa";
        std::string res = plugin.invoke<std::string (std::string&)>("testGL.testString1", a);
        EXPECT_EQ(a,"testString1");
        EXPECT_EQ(res,"res " + a);

        a = "aaa";
        res = plugin.invoke<std::string (std::string&)>("testGL.testString1", a);
        EXPECT_EQ(a,"testString1");
        EXPECT_EQ(res,"res " + a);
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << "\n";
        EXPECT_EQ(true,false);
    }

    try
    {//testString2
        using Sign = std::string (std::string&& srv, std::string slv, std::string& sr);
        std::string a = "aaa";
        std::string res = plugin.invoke<Sign>("testGL.testString2", "a", "b", a);
        EXPECT_EQ(a,"ab");
        EXPECT_EQ(res,"res baab");
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << "\n";
        EXPECT_EQ(true,false);
    }

    try
    {//testHandler
        graft::Router::vars_t vars;
        graft::Input input;
        graft::GlobalContextMap m;
        graft::Context ctx(m);
        graft::Output output;
        using Handler = graft::Status(const graft::Router::vars_t& vars, const graft::Input& input, graft::Context& ctx, graft::Output& output);
        graft::Status res = plugin.invoke<Handler>("testGL.testHandler", vars, input, ctx, output);
        EXPECT_EQ(res,graft::Status::Ok);
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << "\n";
        EXPECT_EQ(true,false);
    }

    IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
    EXPECT_EQ(endpoints.size(), 4);
}

TEST(Graftlets, exceptionList)
{
#define VER(a,b) GRAFTLET_MKVER(a,b)
    using GL = graftlet::GraftletLoader;
    {
        GL::setGraftletsExceptionList({});
        GL loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 4);
    }
    {
        GL::setGraftletsExceptionList({ {"myGraftlet", {{VER(4,2), VER(5,1)}, {VER(1,0), VER(1,0)}} } });
        GL loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 4);
    }
    {
        GL::setGraftletsExceptionList({ {"myGraftlet1", {{VER(4,2), VER(5,1)}, {VER(1,0), VER(1,0)}} } });
        GL loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 2);
    }
    {
        GL::setGraftletsExceptionList({ {"myGraftlet", {{VER(4,2), VER(5,1)}, {VER(1,0), VER(1,1)}} },
                                        {"myGraftlet1", {{VER(4,2), VER(5,1)}, {VER(1,0), VER(1,0)}} }
                                      });
        GL loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 0);
    }

    GL::setGraftletsExceptionList({});

#undef VER
}

TEST(Graftlets, checkFwVersion)
{
#define VER(a,b) GRAFTLET_MKVER(a,b)
    using Version = graftlet::GraftletLoader::Version;
    Version fwVersion = graftlet::GraftletLoader::getFwVersion();
    Version save_ver = fwVersion;

    {
        graftlet::GraftletLoader loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 4);
    }

    {
        graftlet::GraftletLoader::setFwVersion( VER(0,5) );
        graftlet::GraftletLoader loader;
        loader.findGraftletsInDirectory("./graftlets", "so");
        IGraftlet::EndpointsVec endpoints = loader.getEndpoints();
        EXPECT_EQ(endpoints.size(), 2);
    }

    graftlet::GraftletLoader::setFwVersion(save_ver);
#undef VER
}
/////////////////////////////////
// GraftServerTest fixture

class GraftServerTest : public ::testing::Test
{
    class GSTest : public graft::GraftServer
    {
    public:
        bool ready() const { return graft::GraftServer::ready(); }
        void stop() { graft::GraftServer::stop(); }
    protected:
        virtual bool initConfigOption(int argc, const char** argv, graft::ConfigOpts& copts) override
        {
            copts.http_address = "0.0.0.0:28690";
            copts.coap_address = "udp://0.0.0.0:18991";
            copts.workers_count = 0;
            copts.worker_queue_len = 0;
            copts.http_connection_timeout = 360;
            copts.timer_poll_interval_ms = 1000;
            copts.upstream_request_timeout = 360;
            copts.cryptonode_rpc_address = "127.0.0.1:28681";
            copts.data_dir = "";
            copts.graftlet_dirs.emplace_back("graftlets");
            copts.lru_timeout_ms = 60000;
            copts.testnet = true;
            copts.stake_wallet_name = "stake-wallet";
            copts.stake_wallet_refresh_interval_ms = 50000;
            copts.watchonly_wallets_path = "";

            return true;
        }
    };

    GSTest gserver;
    std::thread th;
private:
    void run()
    {
        th = std::thread([this]{ gserver.run(start_args.argc, start_args.argv); });
        while(!gserver.ready())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    void stop_and_wait_for()
    {
        gserver.stop();
        th.join();
    }
protected:
    GraftServerTest()
    {
        run();
    }

    ~GraftServerTest()
    {
        stop_and_wait_for();
    }
protected:
    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }
};

TEST_F(GraftServerTest, graftlets)
{
    std::string json_data = "something";

    {
        //curl method
        std::string res = GraftServerTestBase::send_request("http://127.0.0.1:28690/URI/test/123", json_data);
        EXPECT_EQ(res, json_data+"123");

        //mongoose method
        GraftServerTestBase::Client client;
        client.serve("http://127.0.0.1:28690/URI/test/123", "", json_data);
        EXPECT_EQ(false, client.get_closed());
        std::string res1 = client.get_body();
        EXPECT_EQ(res1, json_data+"123");
    }

    {
        //curl method
        std::string res = GraftServerTestBase::send_request("http://127.0.0.1:28690/URI1/test1/123", json_data);
        EXPECT_EQ(res, json_data+"123");

        //mongoose method
        GraftServerTestBase::Client client;
        client.serve("http://127.0.0.1:28690/URI1/test/123", "", json_data);
        EXPECT_EQ(false, client.get_closed());
        std::string res1 = client.get_body();
        EXPECT_EQ(res1, json_data+"123");
    }
}