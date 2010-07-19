#include "mheads.h"
#include "ouser.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
	CGI *cgi = NULL;
	NEOERR *err;
	mdb_conn *conn = NULL;

	/* skey, user, pass, return jsoncallback*/
	char *s, *u, *r, *jcbk;
	
	//sleep(20);
	mtc_init("logout");
	mconfig_parse_file(SITE_CONFIG, &g_cfg);
	mutil_wrap_fcgi(argc, argv, envp);
	if (mdb_init(&conn, DB_DSN) != MDB_ERR_NONE) {
		mtc_err("init db error %s", mdb_get_errmsg(conn));
		printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
		printf("{errcode: %d}", SAM_ERR_INIT);
		return 1;
	}
	
#ifndef DROP_FCGI
	while (FCGI_Accept() >= 0) {
#endif

		/*
		 * cgi init 
		 */
		err = cgi_init(&cgi, NULL);
		if (err != STATUS_OK) {
			mtc_err("init cgi error");
			printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
			printf("{errcode: %d}", SAM_ERR_INIT);
			goto opfinish;
		}
		err = cgi_parse(cgi);
		if (err != STATUS_OK) {
			mtc_err("parse cgi error");
			hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_PARSE);
			goto opfinish;
		}

		u = hdf_get_value(cgi->hdf, PRE_COOKIE".samuser", NULL);
		s = hdf_get_value(cgi->hdf, PRE_COOKIE".samkey", NULL);
		if (s && u) {
			if (user_has_login(conn, u, s)) {
				user_logout(conn, u);
				cgi_cookie_clear(cgi, "samuser", SITE_DOMAIN, NULL);
				cgi_cookie_clear(cgi, "samkey", SITE_DOMAIN, NULL);
				hdf_set_value(cgi->hdf, PRE_OUTPUT".success", "1");
				goto opfinish;
			}
		}
		hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_NOTLOGIN);
	opfinish:
		if (cgi) {
			r = hdf_get_value(cgi->hdf, PRE_QUERY".r", NULL);
			if (r) {
				cgi_redirect(cgi, r);
			} else {
				jcbk = hdf_get_value(cgi->hdf, PRE_QUERY".jsoncallback", NULL);
				if (jcbk != NULL) {
					mjson_execute_hdf(cgi->hdf, jcbk, 0);
				} else {
					mjson_output_hdf(cgi->hdf, 0);
				}
			}
#ifdef DEBUG_HDF
			hdf_write_file(cgi->hdf, HF_LOG_PATH"hdf.logout");
#endif
			cgi_destroy(&cgi);
		}
		
#ifndef DROP_FCGI
 	} /* FCGI_Accept() */
#endif

	mdb_destroy(conn);
	return 0;
}
