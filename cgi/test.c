#include "mheads.h"
#include "ouser.h"

#define F_TPL_TEST "test.html"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
	CGI *cgi = NULL;
	NEOERR *err;
	mdb_conn *conn = NULL;

	char *s, *u, *p, *r, *jcbk;

	mtc_init("test");
	mconfig_parse_file(SITE_CONFIG, &g_cfg);

	if (mdb_init(&conn, DB_DSN) != MDB_ERR_NONE) {
		mtc_err("init db error %s", mdb_get_errmsg(conn));
		printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
		printf("{errcode: %d}", SAM_ERR_INIT);
		return 1;
	}
	
	err = cgi_init(&cgi, NULL);
	if (err != STATUS_OK) {
		mtc_err("init cgi error");
		printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
		printf("初始化错误");
		return 1;
	}
	err = cgi_parse(cgi);
	if (err != STATUS_OK) {
		mtc_err("parse cgi error");
		hdf_set_value(cgi->hdf, PRE_OUTPUT".errmsg", "初始化出错");
		goto finish;
	}

	u = hdf_get_value(cgi->hdf, PRE_COOKIE".samuser", NULL);
	s = hdf_get_value(cgi->hdf, PRE_COOKIE".samkey", NULL);
	if (s && u) {
		if (user_has_login(conn, u, s)) {
			hdf_set_copy(cgi->hdf, PRE_OUTPUT".samuser", PRE_COOKIE".samuser");
			hdf_set_copy(cgi->hdf, PRE_OUTPUT".samkey", PRE_COOKIE".samkey");
			hdf_set_value(cgi->hdf, PRE_OUTPUT".rcode", "1");
			goto finish;
		}
	}

	hdf_set_value(cgi->hdf, PRE_OUTPUT".rcode", "2");

finish:
	cgi_display(cgi, F_TPL_TEST);
	cgi_destroy(&cgi);
	mdb_destroy(conn);
	return 0;
}
