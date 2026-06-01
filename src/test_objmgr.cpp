#include "handler.hpp"
#include "types.hpp"

#include <zephyr/kernel.h>

#include <printk_thread.h>
#include <task_def.hpp>

#include <string>
#include <vector>

#define PREFIX "[OBJMGR_TEST] "

static int test_passed;
static int test_failed;

#define CHECK(cond, fmt, ...) do { \
    if (cond) { test_passed++; printk_thread(PREFIX "PASS: %s: " fmt, __func__, ##__VA_ARGS__); } \
    else { test_failed++; printk_thread(PREFIX "FAIL: %s: " fmt, __func__, ##__VA_ARGS__); } \
} while (0)

static InterfaceMapType imap()
{
    InterfaceMapType m;
    m["/test/a"] = {{"c0", {"i0"}}};
    m["/test/a/b"] = {{"c1", {"i1"}}};
    m["/test/a/b/c"] = {{"c2", {"i2"}}};
    m["/test/a/b/c/d"] = {{"c3", {"i3"}}};
    return m;
}

static AssociationMaps amap()
{
    AssociationMaps a;
    a.ifaces["/test/a/desc"] = {
        std::shared_ptr<sdbusplus::asio::dbus_interface>(),
        {"/test/a/b", "/test/a/b/c"}
    };
    return a;
}

static void t_new_entry()
{
    std::vector<InterfaceMapType::value_type> r;
    addObjectMapResult(r, "/x", {"s", {"i"}});
    CHECK(r.size() == 1, "got %zu", r.size());
}

static void t_merge()
{
    std::vector<InterfaceMapType::value_type> r;
    addObjectMapResult(r, "/x", {"s1", {"a"}});
    addObjectMapResult(r, "/x", {"s2", {"b"}});
    CHECK(r.size() == 1, "got %zu", r.size());
}

static void t_obj_ok()
{
    auto m = imap(); std::vector<std::string> ifs;
    try {
        auto r = getObject(m, "/test/a", ifs);
        CHECK(r.size() == 1, "got %zu", r.size());
        CHECK(r.contains("c0"), "no c0");
    } catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_obj_nf()
{
    auto m = imap(); std::vector<std::string> ifs;
    bool ok = false;
    try { getObject(m, "/no", ifs); } catch (const std::exception&) { ok = true; }
    CHECK(ok, "expected throw");
}

static void t_obj_fm()
{
    auto m = imap(); std::vector<std::string> ifs = {"i0"};
    try { CHECK(!getObject(m, "/test/a", ifs).empty(), "empty"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_obj_fn()
{
    auto m = imap(); std::vector<std::string> ifs = {"no"};
    bool ok = false;
    try { getObject(m, "/test/a", ifs); } catch (const std::exception&) { ok = true; }
    CHECK(ok, "expected throw");
}

static void t_sub_root()
{
    auto m = imap(); std::vector<std::string> ifs;
    try { CHECK(getSubTree(m, "/", 0, ifs).size() == 4, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_sub_d1()
{
    auto m = imap(); std::vector<std::string> ifs;
    try {
        auto r = getSubTree(m, "/test/a", 1, ifs);
        CHECK(r.size() == 1, "got %zu", r.size());
        if (r.size()) CHECK(r[0].first == "/test/a/b", "got %s", r[0].first.c_str());
    } catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_sub_f()
{
    auto m = imap(); std::vector<std::string> ifs = {"i1", "i3"};
    try { CHECK(getSubTree(m, "/test/a", 0, ifs).size() == 2, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_sub_nf()
{
    auto m = imap(); std::vector<std::string> ifs;
    bool ok = false;
    try { getSubTree(m, "/no", 0, ifs); } catch (const std::exception&) { ok = true; }
    CHECK(ok, "expected throw");
}

static void t_sp_f()
{
    auto m = imap(); std::vector<std::string> ifs = {"i1", "i3"};
    try { CHECK(getSubTreePaths(m, "/test/a", 0, ifs).size() == 2, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_sp_nf()
{
    auto m = imap(); std::vector<std::string> ifs;
    bool ok = false;
    try { getSubTreePaths(m, "/no", 0, ifs); } catch (const std::exception&) { ok = true; }
    CHECK(ok, "expected throw");
}

static void t_anc_ok()
{
    auto m = imap(); std::vector<std::string> ifs = {"i0", "i1"};
    try {
        auto r = getAncestors(m, "/test/a/b/c", ifs);
        CHECK(r.size() == 2, "got %zu", r.size());
        if (r.size() >= 2) {
            CHECK(r[0].first == "/test/a", "got %s", r[0].first.c_str());
            CHECK(r[1].first == "/test/a/b", "got %s", r[1].first.c_str());
        }
    } catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_anc_tr()
{
    auto m = imap(); std::vector<std::string> ifs = {"i0", "i1"};
    try { CHECK(getAncestors(m, "/test/a/b/c/", ifs).size() == 2, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_anc_nf()
{
    auto m = imap(); std::vector<std::string> ifs;
    bool ok = false;
    try { getAncestors(m, "/no", ifs); } catch (const std::exception&) { ok = true; }
    CHECK(ok, "expected throw");
}

static void t_anc_nm()
{
    auto m = imap(); std::vector<std::string> ifs = {"no"};
    try { CHECK(getAncestors(m, "/test/a/b/c", ifs).empty(), "not empty"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_assoc_ok()
{
    auto m = imap(); auto a = amap();
    sdbusplus::message::object_path ap("/test/a/desc");
    sdbusplus::message::object_path rp("/test/a");
    std::vector<std::string> ifs = {"i1", "i2"};
    try { CHECK(getAssociatedSubTree(m, a, ap, rp, 0, ifs).size() == 2, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void t_assoc_no()
{
    auto m = imap(); auto a = amap();
    sdbusplus::message::object_path ap("/test/a/no");
    sdbusplus::message::object_path rp("/test/a");
    std::vector<std::string> ifs = {"i1"};
    CHECK(getAssociatedSubTree(m, a, ap, rp, 0, ifs).empty(), "not empty");
}

static void t_assocp_ok()
{
    auto m = imap(); auto a = amap();
    sdbusplus::message::object_path ap("/test/a/desc");
    sdbusplus::message::object_path rp("/test/a");
    std::vector<std::string> ifs = {"i1", "i2"};
    try { CHECK(getAssociatedSubTreePaths(m, a, ap, rp, 0, ifs).size() == 2, "bad count"); }
    catch (const std::exception& e) { CHECK(false, "exc: %s", e.what()); }
}

static void run()
{
    test_passed = test_failed = 0;
    printk_thread(PREFIX "=== OBJMGR Test Suite ===");

    printk_thread(PREFIX "-- addObjectMapResult --");
    t_new_entry(); t_merge(); k_msleep(100);

    printk_thread(PREFIX "-- getObject --");
    t_obj_ok(); t_obj_nf(); t_obj_fm(); t_obj_fn(); k_msleep(100);

    printk_thread(PREFIX "-- getSubTree --");
    t_sub_root(); t_sub_d1(); t_sub_f(); t_sub_nf(); k_msleep(100);

    printk_thread(PREFIX "-- getSubTreePaths --");
    t_sp_f(); t_sp_nf(); k_msleep(100);

    printk_thread(PREFIX "-- getAncestors --");
    t_anc_ok(); t_anc_tr(); t_anc_nf(); t_anc_nm(); k_msleep(100);

    printk_thread(PREFIX "-- getAssociatedSubTree/Paths --");
    t_assoc_ok(); t_assoc_no(); t_assocp_ok(); k_msleep(100);

    int n = test_passed + test_failed;
    printk_thread(PREFIX "=== Result: %d/%d passed, %d failed ===",
           test_passed, n, test_failed);
}

static void* objmgr_test_handler(void*)
{
    extern struct k_sem objmgr_ready_sem;
    // k_sem_take(&objmgr_ready_sem, K_FOREVER);
    printk_thread(PREFIX "objmgr ready, running tests");
    run();
    if (test_failed > 0) {
        printk_thread(PREFIX "WARNING: %d test(s) FAILED!", test_failed);
    }
    return nullptr;
}

#define OBJMGR_TEST_STACK_SIZE (16 * 1024)

int objmgr_test_init(void)
{
    static pthread_t thread;
    printk_thread(PREFIX "creating test thread");
    CREATE_TASK_WITH_PTHREAD(&thread, objmgr_test, OBJMGR_TEST_STACK_SIZE);
    return 0;
}