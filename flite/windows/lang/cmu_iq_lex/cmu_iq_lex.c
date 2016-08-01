/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2013                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  iq Lexical function                                            */
/*                                                                       */
/*************************************************************************/
#include "flite.h"
#include "cst_val.h"
#include "cst_voice.h"
#include "cst_lexicon.h"
#include "cst_ffeatures.h"
#include "cmu_iq_lex.h"

extern const int cmu_iq_lex_entry[];
extern const unsigned char cmu_iq_lex_data[];
extern const int cmu_iq_lex_num_entries;
extern const int cmu_iq_lex_num_bytes;
extern const char * const cmu_iq_lex_phone_table[54];
extern const char * const cmu_iq_lex_phones_huff_table[];
extern const char * const cmu_iq_lex_entries_huff_table[];

static int cmu_iq_is_vowel(const char *p);
static int cmu_iq_is_silence(const char *p);
static int cmu_iq_has_vowel_in_list(const cst_val *v);
static int cmu_iq_has_vowel_in_syl(const cst_item *i);

static const char * const addenda0[] = { "p,", NULL };
static const char * const addenda1[] = { "p.", NULL };
static const char * const * const addenda[] = {
    addenda0,
    addenda1,
    NULL };

cst_utterance *cmu_iq_postlex(cst_utterance *u)
{
    /* Post lexical rules */

    return u;
}

static int cmu_iq_is_silence(const char *p)
{
    if (cst_streq(p,"pau"))
	return TRUE;
    else
	return FALSE;
}

static int cmu_iq_has_vowel_in_list(const cst_val *v)
{
    const cst_val *t;

    for (t=v; t; t=val_cdr(t))
	if (cmu_iq_is_vowel(val_string(val_car(t))))
	    return TRUE;
    return FALSE;
}

static int cmu_iq_has_vowel_in_syl(const cst_item *i)
{
    const cst_item *n;

    for (n=i; n; n=item_prev(n))
	if (cmu_iq_is_vowel(item_feat_string(n,"name")))
	    return TRUE;
    return FALSE;
}

static int cmu_iq_is_vowel(const char *p)
{
    /* this happens to work for US English phoneset */
    if (strchr("aeiouAEIOU",p[0]) == NULL)
	return FALSE;
    else
	return TRUE;
}

static int cmulex_dist_to_vowel(const cst_val *rest)
{
    if (rest == 0)
        return 0;  /* shouldn't get here */
    else if (cmu_iq_is_vowel(val_string(val_car(rest))))
        return 0;
    else
        return 1+cmulex_dist_to_vowel(val_cdr(rest));
}

int cmu_iq_syl_boundary_mo(const cst_item *i,const cst_val *rest)
{
    /* syl boundary maximal onset */
    int d2v;

    if (rest == NULL)
	return TRUE;
    else if (cmu_iq_is_silence(val_string(val_car(rest))))
	return TRUE;
    else if (!cmu_iq_has_vowel_in_list(rest)) 
        /* no more vowels so rest *all* coda */
	return FALSE;
    else if (!cmu_iq_has_vowel_in_syl(i))  /* need a vowel */
        /* no vowel yet in syl so keep copying */
	return FALSE;
    else if (cmu_iq_is_vowel(val_string(val_car(rest))))
        /* next is a vowel, syl has vowel, so this is a break */
	return TRUE;
    else 
    {
        /* want to know if from rest to the next vowel is a valid onset */
        d2v = cmulex_dist_to_vowel(rest);
        if (d2v < 2)
            return TRUE;
        else if (d2v > 3)
            return FALSE;
#if 0
        else if (d2v == 2) 
            return cmulex_onset_bigram(rest);
        else /* if (d2v == 3) */
            return cmulex_onset_trigram(rest);
#endif
        return TRUE;
    }

}

cst_val *cmu_iq_lts_function(const struct lexicon_struct *l, 
                                   const char *word, const char *pos)
{
    cst_val *phones = 0;
    cst_val *utflets = 0;

    /* Need to explode word as a utf-string */
    utflets = cst_utf8_explode(word);

    /* Do lts */
    phones = lts_apply_val(utflets,
                           "",  /* more features if we had them */
                           l->lts_rule_set);
#if 0
    printf("awb_debug iq OOV \"%s\" ",word);
    val_print(stdout,phones);
    printf("\n");
#endif

    delete_val(utflets);

    return phones;
}

cst_lexicon cmu_iq_lex;
cst_lts_rules cmu_iq_lts_rules;
extern const char * const cmu_iq_lts_phone_table[];
extern const char * const cmu_iq_lts_letter_table[];
extern const cst_lts_addr cmu_iq_lts_letter_index[];
extern const cst_lts_model cmu_iq_lts_model[];

cst_lexicon *cmu_iq_lex_init(void)
{
    /* Should it be global const or dynamic */
    /* Can make lts_rules just a cart tree like others */
    if (cmu_iq_lts_rules.name)
        return &cmu_iq_lex;  /* Already initialized */

    cmu_iq_lts_rules.name = "cmu_iq";
    cmu_iq_lts_rules.letter_index = cmu_iq_lts_letter_index;
#ifdef CST_NO_STATIC_LTS_MODEL
    /* cmu_iq_lts_rules.models will be set elsewhere */
#else
    cmu_iq_lts_rules.models = cmu_iq_lts_model;
#endif
    cmu_iq_lts_rules.phone_table = cmu_iq_lts_phone_table;
    cmu_iq_lts_rules.context_window_size = 4;
    cmu_iq_lts_rules.context_extra_feats = 1;
    cmu_iq_lts_rules.letter_table = cmu_iq_lts_letter_table;

    cmu_iq_lex.name = "cmu_iq_lex";
    cmu_iq_lex.num_entries = cmu_iq_lex_num_entries;
#ifdef CST_NO_STATIC_LEX
    /* cmu_iq_lex.data will be set elsewhere */
#else
    cmu_iq_lex.data = cmu_iq_lex_data;
#endif
    cmu_iq_lex.num_bytes = cmu_iq_lex_num_bytes;
    cmu_iq_lex.phone_table = (char **) cmu_iq_lex_phone_table;
    cmu_iq_lex.syl_boundary = cmu_iq_syl_boundary_mo;
    cmu_iq_lex.addenda = (char ***) addenda;
    cmu_iq_lex.lts_rule_set = (cst_lts_rules *) &cmu_iq_lts_rules;

    cmu_iq_lex.lts_function = cmu_iq_lts_function;

    cmu_iq_lex.phone_hufftable = cmu_iq_lex_phones_huff_table;
    cmu_iq_lex.entry_hufftable = cmu_iq_lex_entries_huff_table;

    cmu_iq_lex.postlex = cmu_iq_postlex;

    return &cmu_iq_lex;  /* Already initialized */

}
