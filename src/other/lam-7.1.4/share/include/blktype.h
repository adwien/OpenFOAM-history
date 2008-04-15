/*
 * Copyright (c) 2001-2002 The Trustees of Indiana University.  
 *                         All rights reserved.
 * Copyright (c) 1998-2001 University of Notre Dame. 
 *                         All rights reserved.
 * Copyright (c) 1994-1998 The Ohio State University.  
 *                         All rights reserved.
 * 
 * This file is part of the LAM/MPI software package.  For license
 * information, see the LICENSE file in the top level directory of the
 * LAM/MPI source distribution.
 * 
 * $HEADER$
 *
 *	$Id: blktype.h,v 6.12 2002/10/09 20:57:22 brbarret Exp $
 *
 *	Function:	- LAM MPI blocking request type codes
 */

#ifndef _BLKTYPE
#define _BLKTYPE

#include <lam_config.h>
#include "lam.h"

#define BLKLOW			((int4) 1)	/* lowest valid block type */

#define BLKMPIBUFATTACH		((int4) 1)
#define BLKMPIBUFDETACH		((int4) 2)
#define BLKMPIINITED		((int4) 3)
#define BLKMPIABORT		((int4) 4)
#define BLKMPIADDRESS		((int4) 5)
#define BLKMPIALLGATHER		((int4) 6)
#define BLKMPIALLGATHERV	((int4) 7)
#define BLKMPIALLREDUCE		((int4) 8)
#define BLKMPIALLTOALL		((int4) 9)
#define BLKMPIALLTOALLV		((int4) 10)
#define BLKMPIATTRDEL		((int4) 11)
#define BLKMPIATTRGET		((int4) 12)
#define BLKMPIATTRPUT		((int4) 13)
#define BLKMPIBARRIER		((int4) 14)
#define BLKMPIBCAST		((int4) 15)
#define BLKMPIBSEND		((int4) 16)
#define BLKMPIBSINIT		((int4) 17)
#define BLKMPICANCEL		((int4) 18)
#define BLKMPICARTCOORDS	((int4) 19)
#define BLKMPICARTCREATE	((int4) 20)
#define BLKMPICARTDIMGET	((int4) 21)
#define BLKMPICARTGET		((int4) 22)
#define BLKMPICARTMAP		((int4) 23)
#define BLKMPICARTRANK		((int4) 24)
#define BLKMPICARTSHIFT		((int4) 25)
#define BLKMPICARTSUB		((int4) 26)
#define BLKMPICOMMCMP		((int4) 27)
#define BLKMPICOMMCREATE	((int4) 28)
#define BLKMPICOMMDUP		((int4) 29)
#define BLKMPICOMMFREE		((int4) 30)
#define BLKMPICOMMGROUP		((int4) 31)
#define BLKMPICOMMRANK		((int4) 32)
#define BLKMPICOMMRGROUP	((int4) 33)
#define BLKMPICOMMRSIZE		((int4) 34)
#define BLKMPICOMMSIZE		((int4) 35)
#define BLKMPICOMMSPLIT		((int4) 36)
#define BLKMPICOMMTESTINTER	((int4) 37)
#define BLKMPIDIMSCREATE	((int4) 38)
#define BLKMPIERRCLASS		((int4) 39)
#define BLKMPIERRCREATE		((int4) 40)
#define BLKMPIERRFREE		((int4) 41)
#define BLKMPIERRGET		((int4) 42)
#define BLKMPIERRSET		((int4) 43)
#define BLKMPIERRSTRING		((int4) 44)
#define BLKMPIFINAL		((int4) 45)
#define BLKMPIGATHER		((int4) 46)
#define BLKMPIGATHERV		((int4) 47)
#define BLKMPIGETCOUNT		((int4) 48)
#define BLKMPIGETELEM		((int4) 49)
#define BLKMPIGETPNAME		((int4) 50)
#define BLKMPIGRAPHCREATE	((int4) 51)
#define BLKMPIGRAPHDIMSGET	((int4) 52)
#define BLKMPIGRAPHGET		((int4) 53)
#define BLKMPIGRAPHMAP		((int4) 54)
#define BLKMPIGRAPHNBR		((int4) 55)
#define BLKMPIGRAPHNBRCOUNT	((int4) 56)
#define BLKMPIGROUPCMP		((int4) 57)
#define BLKMPIGROUPDIFF		((int4) 58)
#define BLKMPIGROUPEXCL		((int4) 59)
#define BLKMPIGROUPFREE		((int4) 60)
#define BLKMPIGROUPINCL		((int4) 61)
#define BLKMPIGROUPINTER	((int4) 62)
#define BLKMPIGROUPRANK		((int4) 63)
#define BLKMPIGROUPREXCL	((int4) 64)
#define BLKMPIGROUPRINCL	((int4) 65)
#define BLKMPIGROUPSIZE		((int4) 66)
#define BLKMPIGROUPTRANKS	((int4) 67)
#define BLKMPIGROUPUNION	((int4) 68)
#define BLKMPIIBSEND		((int4) 69)
#define BLKMPIICOMMCREATE	((int4) 70)
#define BLKMPIICOMMMERGE	((int4) 71)
#define BLKMPIINIT		((int4) 72)
#define BLKMPIIPROBE		((int4) 73)
#define BLKMPIIRECV		((int4) 74)
#define BLKMPIIRSEND		((int4) 75)
#define BLKMPIISEND		((int4) 76)
#define BLKMPIISSEND		((int4) 77)
#define BLKMPIKEYCREATE		((int4) 78)
#define BLKMPIKEYFREE		((int4) 79)
#define BLKMPIOPCREATE		((int4) 80)
#define BLKMPIOPFREE		((int4) 81)
#define BLKMPIPACK		((int4) 82)
#define BLKMPIPACKSIZE		((int4) 83)
#define BLKMPIPROBE		((int4) 84)
#define BLKMPIRECV		((int4) 85)
#define BLKMPIREDUCE		((int4) 86)
#define BLKMPIREDUCESCATTER	((int4) 87)
#define BLKMPIREQFREE		((int4) 88)
#define BLKMPIRINIT		((int4) 89)
#define BLKMPIRSEND		((int4) 90)
#define BLKMPIRSINIT		((int4) 91)
#define BLKMPISCAN		((int4) 92)
#define BLKMPISCATTER		((int4) 93)
#define BLKMPISCATTERV		((int4) 94)
#define BLKMPISEND		((int4) 95)
#define BLKMPISENDRECV		((int4) 96)
#define BLKMPISENDRECVREP	((int4) 97)
#define BLKMPISINIT		((int4) 98)
#define BLKMPISSEND		((int4) 99)
#define BLKMPISSINIT		((int4) 100)
#define BLKMPISTART		((int4) 101)
#define BLKMPISTARTALL		((int4) 102)
#define BLKMPITCOMMIT		((int4) 103)
#define BLKMPITCONTIG		((int4) 104)
#define BLKMPIPCONTROL		((int4) 105)
#define BLKMPITEST		((int4) 106)
#define BLKMPITESTALL		((int4) 107)
#define BLKMPITESTANY		((int4) 108)
#define BLKMPITESTCANCEL	((int4) 109)
#define BLKMPITESTSOME		((int4) 110)
#define BLKMPITEXTENT		((int4) 111)
#define BLKMPITFREE		((int4) 112)
#define BLKMPITHINDEX		((int4) 113)
#define BLKMPITHVECTOR		((int4) 114)
#define BLKMPITINDEX		((int4) 115)
#define BLKMPITLB		((int4) 116)
#define BLKMPITOPOTEST		((int4) 117)
#define BLKMPITSIZE		((int4) 118)
#define BLKMPITSTRUCT		((int4) 119)
#define BLKMPITUB		((int4) 120)
#define BLKMPITVECTOR		((int4) 121)
#define BLKMPIUNPACK		((int4) 122)
#define BLKMPIWAIT		((int4) 123)
#define BLKMPIWAITALL		((int4) 124)
#define BLKMPIWAITANY		((int4) 125)
#define BLKMPIWAITSOME		((int4) 126)

#define BLKMPILSPAWN		((int4) 127)
#define BLKMPILCOMMPARENT	((int4) 128)
#define BLKMPILTRACEON		((int4) 129)
#define BLKMPILTRACEOFF		((int4) 130)
#define BLKMPILCOMMID		((int4) 131)
#define BLKMPILCOMMGPS		((int4) 132)
#define BLKMPILTYPEID		((int4) 133)
#define BLKMPILSIGNAL		((int4) 134)
#define BLKMPILUNIVERSESIZE	((int4) 135)

#define BLKMPIGETVERSION	((int4) 136)
#define BLKMPIWTICK		((int4) 137)
#define BLKMPIWTIME		((int4) 138)

#define BLKMPICOMMC2F		((int4) 139)
#define BLKMPITYPEC2F		((int4) 140)
#define BLKMPIGROUPC2F		((int4) 141)
#define BLKMPIOPC2F		((int4) 142)
#define BLKMPIREQC2F		((int4) 143)
#define BLKMPICOMMF2C		((int4) 144)
#define BLKMPITYPEF2C		((int4) 145)
#define BLKMPIGROUPF2C		((int4) 146)
#define BLKMPIOPF2C		((int4) 147)
#define BLKMPIREQF2C		((int4) 148)
#define BLKMPISTATC2F		((int4) 149)
#define BLKMPISTATF2C		((int4) 150)

#define BLKMPITTEXTENT		((int4) 151)
#define BLKMPITGETENV		((int4) 152)
#define BLKMPITGETCONT		((int4) 153)
#define BLKMPIFINALIZED		((int4) 154)

#define BLKMPIACCUMULATE	((int4) 155)
#define BLKMPIADDERRCLASS	((int4) 156)
#define BLKMPIALLOCMEM		((int4) 157)
#define BLKMPIFILEREADALL	((int4) 158)
#define BLKMPIALLTOALLW		((int4) 159)
#define BLKMPICLOSEPORT		((int4) 160)
#define BLKMPICOMMCALLERRHDL 	((int4) 161)
#define BLKMPICOMMCLONE		((int4) 162)
#define BLKMPICOMMCONNECT	((int4) 163)
#define BLKMPICOMMCREATEERRHDL	((int4) 164)
#define BLKMPICOMMCREATEKEYVAL	((int4) 165)
#define BLKMPICOMMDISCONNECT	((int4) 166)
#define BLKMPICOMMDUPFN		((int4) 167)
#define BLKMPICOMMFREEKEYVAL	((int4) 168)
#define BLKMPICOMMGETERRHDL	((int4) 169)
#define BLKMPICOMMGETNAME 	((int4) 170)
#define BLKMPICOMMGETPARENT	((int4) 171)
#define BLKMPICOMMJOIN		((int4) 172)
#define BLKMPICOMMNULLCOPYFN	((int4) 173)
#define BLKMPICOMMSETERRHDL	((int4) 174)
#define BLKMPICOMMSETNAME 	((int4) 175)
#define BLKMPICOMMSPAWN		((int4) 176)
#define BLKMPICOMMSPAWNMULT	((int4) 177)
#define BLKMPIEXSCAN		((int4) 178)
#define BLKMPIFILEC2F		((int4) 179)
#define BLKMPIFILECALLERRHDL 	((int4) 180)
#define BLKMPIFILECLOSE		((int4) 181)
#define BLKMPIFILECREATEERRHDL	((int4) 182)
#define BLKMPIFILEDELETE	((int4) 183)
#define BLKMPIFILEF2C		((int4) 184)
#define BLKMPIFILEGETAMODE	((int4) 185)
#define BLKMPIFILEGETATOM	((int4) 186)
#define BLKMPIFILEGETBYTEOFFSET	((int4) 187)
#define BLKMPIFILEGETERRHDL	((int4) 188)
#define BLKMPIFILEGETGROUP	((int4) 189)
#define BLKMPIFILEGETINFO	((int4) 190)
#define BLKMPIFILEGETPOS	((int4) 191)
#define BLKMPIFILEGETPOSSHARED	((int4) 192)
#define BLKMPIFILEGETSIZE	((int4) 193)
#define BLKMPIFILEGETTEXTENT	((int4) 194)
#define BLKMPIFILEGETVIEW	((int4) 195)
#define BLKMPIFILEIREAD		((int4) 196)
#define BLKMPIFILEIREADAT	((int4) 197)
#define BLKMPIFILEIREADSHARED	((int4) 198)
#define BLKMPIFILEIWRITE	((int4) 199)
#define BLKMPIFILEIWRITEAT	((int4) 200)
#define BLKMPIFILEIWRITESHARED	((int4) 201)
#define BLKMPIFILEOPEN		((int4) 202)
#define BLKMPIFILEPREALLOCATE	((int4) 203)
#define BLKMPIFILEREAD		((int4) 204)
#define BLKMPIFILEREADALLBEG	((int4) 205)
#define BLKMPIFILEREADAT	((int4) 206)
#define BLKMPIFILEREADATALLBEG	((int4) 207)
#define BLKMPIFILEREADORD	((int4) 208)
#define BLKMPIFILEREADORDBEG	((int4) 209)
#define BLKMPIFILEREADSHARED	((int4) 210)
#define BLKMPIFILESEEK		((int4) 211)
#define BLKMPIFILESEEKSHARED	((int4) 212)
#define BLKMPIFILESETATOM	((int4) 213)
#define BLKMPIFILESETERRHDL	((int4) 214)
#define BLKMPIFILESETINFO	((int4) 215)
#define BLKMPIFILESETSIZE	((int4) 216)
#define BLKMPIFILESETVIEW	((int4) 217)
#define BLKMPIFILESYNC		((int4) 218)
#define BLKMPIFILEWRITE		((int4) 219)
#define BLKMPIFILEWRITEALLBEG	((int4) 220)
#define BLKMPIFILEWRITEAT	((int4) 221)
#define BLKMPIFILEWRITEATALLBEG	((int4) 222)
#define BLKMPIFILEWRITEORD	((int4) 223)
#define BLKMPIFILEWRITEORDBEG	((int4) 224)
#define BLKMPIFILEWRITESHARED	((int4) 225)
#define BLKMPIFREEMEM		((int4) 226)
#define BLKMPIGET		((int4) 227)
#define BLKMPIGETADDRESS	((int4) 228)
#define BLKMPIGREQCOMPLETE	((int4) 229)
#define BLKMPIINFOCREATE	((int4) 230)
#define BLKMPIINFOF2C		((int4) 231)
#define BLKMPIINFOC2F		((int4) 232)
#define BLKMPIINFOFREE		((int4) 233)
#define BLKMPIINITTHREAD	((int4) 234)
#define BLKMPILOOKUPNAME	((int4) 235)
#define BLKMPIOPENPORT		((int4) 236)
#define BLKMPIPACKEXTERNAL	((int4) 237)
#define BLKMPIPUBLISHNAME	((int4) 238)
#define BLKMPIPUT		((int4) 239)
#define BLKMPIQUERYTHREAD	((int4) 240)
#define BLKMPIREGISTERDATAREP	((int4) 241)
#define BLKMPIREQGETSTAT	((int4) 242)
#define BLKMPISIZEOF		((int4) 243)
#define BLKMPISTATSETCANCELLED	((int4) 244)
#define BLKMPITCREATEDARRAY	((int4) 245)
#define BLKMPITCREATEF90COMPLEX	((int4) 246)
#define BLKMPITCREATEHINDEX	((int4) 247)
#define BLKMPITCREATEINDEXBLOCK	((int4) 248)
#define BLKMPITCREATEKEYVAL	((int4) 249)
#define BLKMPITCREATERESIZED	((int4) 250)
#define BLKMPITCREATESTRUCT	((int4) 251)
#define BLKMPITCREATESUBARRAY	((int4) 252)
#define BLKMPITDELETEATTR	((int4) 253)
#define BLKMPITDUP		((int4) 254)
#define BLKMPITDUPFN		((int4) 255)
#define BLKMPITFREEKEYVAL	((int4) 256)
#define BLKMPITGETEXTENT	((int4) 257)
#define BLKMPITGETNAME 		((int4) 258)
#define BLKMPITMATCHSIZE	((int4) 259)
#define BLKMPITNULLCOPYFN	((int4) 260)
#define BLKMPITSETNAME 		((int4) 261)
#define BLKMPIUNPACKEXTERNAL	((int4) 262)
#define BLKMPIUNPUBLISHNAME	((int4) 263)
#define BLKMPIWINC2F		((int4) 264)
#define BLKMPIWINCALLERRHDL 	((int4) 265)
#define BLKMPIWINCOMPLETE	((int4) 266)
#define BLKMPIWINCREATE		((int4) 267)
#define BLKMPIWINCREATEERRHDL	((int4) 268)
#define BLKMPIWINCREATEKEYVAL	((int4) 269)
#define BLKMPIWINF2C		((int4) 270)
#define BLKMPIWINFENCE		((int4) 271)
#define BLKMPIWINFREE		((int4) 272)
#define BLKMPIWINFREEKEYVAL	((int4) 273)
#define BLKMPIWINGETERRHDL	((int4) 274)
#define BLKMPIWINGETGROUP	((int4) 275)
#define BLKMPIWINGETNAME 	((int4) 276)
#define BLKMPIWINLOCK		((int4) 277)
#define BLKMPIWINNULLCOPYFN	((int4) 278)
#define BLKMPIWINPOST		((int4) 279)
#define BLKMPIWINSETATTR	((int4) 280)
#define BLKMPIWINSETERRHDL	((int4) 281)
#define BLKMPIWINSETNAME 	((int4) 282)
#define BLKMPIWINSTART		((int4) 283)
#define BLKMPIWINTEST		((int4) 284)
#define BLKMPIWINUNLOCK		((int4) 285)
#define BLKMPIWINWAIT		((int4) 286)
#define BLKMPIFILEWRITEALL	((int4) 287)
#define BLKMPIOTEST		((int4) 288)
#define BLKMPIOWAIT		((int4) 289)
#define BLKMPIFILEREADATALL	((int4) 290)
#define BLKMPIFILEWRITEATALL	((int4) 291)
#define BLKMPIINFOSET		((int4) 292)
#define BLKMPIINFODEL		((int4) 293)
#define BLKMPIINFOGET		((int4) 294)
#define BLKMPIINFOGETLEN	((int4) 295)
#define BLKMPIINFOGETNKEYS	((int4) 296)
#define BLKMPIINFOGETNTHKEY	((int4) 297)
#define BLKMPIINFODUP		((int4) 298)
#define BLKMPIISTHREADMAIN	((int4) 299)
#define BLKMPISTATSETELEM	((int4) 300)

#define BLKMPICOMMACCEPT	((int4) 301)
#define BLKMPICOMMGETATTR	((int4) 302)
#define BLKMPICOMMSETATTR	((int4) 303)
#define BLKMPITGETATTR		((int4) 304)
#define BLKMPITSETATTR		((int4) 305)
#define BLKMPIWINGETATTR 	((int4) 306)
#define BLKMPICOMMDELETEATTR	((int4) 307)
#define BLKMPIWINDELETEATTR	((int4) 308)
#define BLKMPITCREATEHVECTOR	((int4) 309)

#define BLKMPIRUN               ((int4) 310)

#define BLKMPILREQUESTGETNAME   ((int4) 311)
#define BLKMPILREQUESTSETNAME   ((int4) 312)
#define BLKXMPIBUOY             ((int4) 313)
#define BLKXMPICOLORON          ((int4) 314)
#define BLKXMPICOLOROFF         ((int4) 315)
#define BLKIMPIDAEMON           ((int4) 316)

#define BLKIMPIBARRIER          ((int4) 317)
#define BLKIMPIBCAST            ((int4) 318)
#define BLKIMPIGATHER           ((int4) 319)
#define BLKIMPIALLGATHER        ((int4) 320)
#define BLKIMPIREDUCE           ((int4) 321)
#define BLKIMPIALLREDUCE        ((int4) 322)

#define BLKHIGH			((int4) 322)	/* highest valid block type */

#ifdef __cplusplus
extern "C" {
#endif

extern char		*blktype __ARGS((int4 type));

#ifdef __cplusplus
}
#endif

#endif /* _BLKTYPE */
