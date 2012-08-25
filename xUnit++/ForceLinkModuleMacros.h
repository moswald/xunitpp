#ifndef FORCELINKMODULEMACROS_H_
#define FORCELINKMODULEMACROS_H_

#define ENABLE_MODULE_LINK(x) namespace force_link_##x { int link_##x; }
#define LINK_MODULE(x) namespace force_link_##x { void do_link_##x() { extern int link_##x; link_##x = 1; } }

#endif
