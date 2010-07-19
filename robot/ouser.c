#include "mheads.h"
#include "ouser.h"

bool user_has_login(mdb_conn *conn, char *uid, char *skey)
{
	if (!conn || !uid || !skey) return false;

	char *s;
	
	mdb_exec(conn, NULL, "SELECT skey from account WHERE userid=$1;", "s", uid);

	if (mdb_get(conn, "s", &s) == MDB_ERR_NONE) {
		if (!strcmp(s, skey))
			return true;
	}
	
	return false;
}

char* user_login_auth(mdb_conn *conn, char *uid, char *pass)
{
	if (!conn || !uid || !pass) return NULL;

	char *p, *r;
	int ret, x;
	
	mdb_exec(conn, NULL, "SELECT password from account WHERE userid=$1;", "s", uid);
	if (mdb_get(conn, "s", &p) == MDB_ERR_NONE) {
		if (!strcmp(p, pass)) {
			
			r = calloc(1, LEN_SKEY+1);
			for (x = 0; x < LEN_SKEY; x++) {
				r[x] = (char)(65 + neo_rand(90-65));
			}
			r[x] = '\0';

			ret = mdb_exec(conn, NULL, "UPDATE account SET skey='$1' WHERE "
						   " userid=$2;", "ss", r, uid);
			if (ret != MDB_ERR_NONE) {
				mtc_err("exec failure %s", mdb_get_errmsg(conn));
				free(r);
				return NULL;
			}
			return r;
		}
	}

	return NULL;
}

void user_logout(mdb_conn *conn, char *uid)
{
	if (!conn || !uid) return;

	mdb_exec(conn, NULL, "UPDATE account set skey='0' WHERE userid=$1", "s", uid);
}
