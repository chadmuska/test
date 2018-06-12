/**************************************************************************
 *
 *
 **************************************************************************/

#ifndef _APP_JET_SD_CERT_H_
#define _APP_JET_SD_CERT_H_



/*
 *  Because it's not feasible to test most of SDXC simulator,
 *  it might be better to set ENABLE_SD_CERT_CHECK to 0 so that if won't
 *  have side effects on the formal release.
 *
 *  It's suggested to set ENABLE_SD_CERT_CHECK to 1 only for the special
 *  testing version for SDXC Certification.
 *  Until the test is mature, then integrate to the formal release.
 *
 */

#define ENABLE_SD_CERT_CHECK    1



/*
 *  Exported Functions
 *
 */

extern BOOL appInfoSD_CertCheck(void);

#endif  /*  _APP_JET_SD_CERT_H_  */

