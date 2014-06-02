/* 
 * File:   mixid.h
 * Author: Aidan Richmond
 * Based On: Ccrc.h by Olaf Van Der Spek
 *
 */

#ifndef MIXID_H
#define	MIXID_H

#include <string>

#ifdef _MSC_VER
#include "win32/stdint.h"
#else
#include <stdint.h>
#endif

typedef enum 
{ 
    game_td,
    game_ra,
    game_ts,
    game_dune2,
    game_dune2000,
    game_ra2,
    game_ra2_yr,
    game_rg,
    game_gr,
    game_gr_zh,
    game_ebfd,
    game_nox,
    game_bfme,
    game_bfme2,
    game_tw,
    game_ts_fs,
    game_unknown
} t_game;

namespace MixID
{
    int32_t idGen(t_game game, std::string fname);
    std::string idStr(int32_t id);
    int32_t strId(std::string hex);
}

#endif	/* CCRC_H */

