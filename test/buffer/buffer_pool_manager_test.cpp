/**
 * buffer_pool_manager_test.cpp
 */

#include <cstdio>

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"

namespace cmudb {

TEST(BufferPoolManagerTest, SampleTest) {
  page_id_t temp_page_id;

  DiskManager *disk_manager = new DiskManager("test.db");
  BufferPoolManager bpm(10, disk_manager);

  auto page_zero = bpm.NewPage(temp_page_id);
  EXPECT_NE(nullptr, page_zero);
  EXPECT_EQ(0, temp_page_id);

  // The test will fail here if the page is null
  ASSERT_NE(nullptr, page_zero);

  // change content in page one
  strcpy(page_zero->GetData(), "Hello");

  for (int i = 1; i < 10; ++i) {
    EXPECT_NE(nullptr, bpm.NewPage(temp_page_id));
  }
  // all the pages are pinned, the buffer pool is full
  for (int i = 10; i < 15; ++i) {
    EXPECT_EQ(nullptr, bpm.NewPage(temp_page_id));
  }
  // upin the first five pages, add them to LRU list, set as dirty
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(true, bpm.UnpinPage(i, true));
  }
  bpm.FlushPage(0);
  char test[PAGE_SIZE];
  disk_manager->ReadPage(0, test);
    // check read content
  EXPECT_EQ(0, strcmp(test, "Hello"));
  // we have 5 empty slots in LRU list, evict page zero out of buffer pool
  for (int i = 10; i < 14; ++i) {
    EXPECT_NE(nullptr, bpm.NewPage(temp_page_id));
  }
  // fetch page one again
  page_zero = bpm.FetchPage(0);
  // check read content
  EXPECT_EQ(0, strcmp(page_zero->GetData(), "Hello"));

  remove("test.db");
}

TEST(BufferPoolManagerTest, CustomeTest1) {



    DiskManager *disk_manager = new DiskManager("test.db");
//    BufferPoolManager bpm(10, disk_manager);
//    page_id_t temp_page_id;
//    auto page_zero = bpm.NewPage(temp_page_id);
//    cout<<page_zero->GetPinCount();
//    auto page_one = bpm.NewPage(temp_page_id);
//    auto page_two = bpm.NewPage(temp_page_id);
//
//    strcpy(page_zero->GetData(), "Hello");
//    strcpy(page_one->GetData(), "");
//    strcpy(page_two->GetData(), "");
//
//    bpm.FlushPage(0);
//    bpm.FlushPage(1);
//    bpm.FlushPage(2);

    // fetch page one again
    char test[PAGE_SIZE];
    char test2[PAGE_SIZE];
    disk_manager->ReadPage(0, test);

    disk_manager->WritePage(0, (char*)"Hello World");
    disk_manager->ReadPage(0, test2);
    // check read content
//    EXPECT_EQ(0, strcmp(test, "Hello"));

    cout<<"=======\n";
    cout<<test2;
    remove("test.db");
}


} // namespace cmudb
