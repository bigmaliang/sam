#ifndef __OUSER_H__
#define __OUSER_H__
#include "mheads.h"

__BEGIN_DECLS

#define SITE_DOMAIN "mangoq.com"
#define SITE_CONFIG "/home/bigml/web/sam/config.hdf"
#define P3P_HEADER	"P3P: CP=\"CURa ADMa DEVa PSAo PSDo OUR BUS UNI PUR INT DEM STA PRE COM NAV OTC NOI DSP COR\"\n"
#define DB_DSN		(hdf_get_value(g_cfg, "Dbsn", NULL))
#define LEN_SKEY	128

enum {
	SAM_ERR_NEEDINPUT = 1,
	SAM_ERR_INIT,
	SAM_ERR_PARSE,
	SAM_ERR_PASSW,
	SAM_ERR_NOTLOGIN,
	SAM_ERR_NEEDREST = 100
} errcode;

bool user_has_login(mdb_conn *conn, char *uid, char *skey);
char* user_login_auth(mdb_conn *conn, char *uid, char *pass);
void user_logout(mdb_conn *conn, char *uid);

__END_DECLS
#endif /* __OUSER_H__ */
