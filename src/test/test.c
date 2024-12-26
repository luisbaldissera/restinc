#include "test.h"
#include "hashtable.h"

#define MAX_TEST_CASES 100

struct HT *__all_tests = NULL;

int TEST_register(const char *casedesc, TESTCase testcase) {
  if (__all_tests == NULL) {
    __all_tests = HT_new();
  }
  HT_set(__all_tests, casedesc, testcase);
  return 0;
}

int TEST_runcase(const char *casedesc) {
  TESTCase testcase = HT_get(__all_tests, casedesc);
  if (testcase == NULL) {
    return -1;
  }
  return testcase();
}

int TEST_runall() {
  char *keys[MAX_TEST_CASES];
  int keysc = HT_keys(__all_tests, keys);
  for (int i = 0; i < keysc; i++) {
    int result = TEST_runcase(keys[i]);
    if (result != 0) {
      return result;
    }
  }
  return 0;
}
