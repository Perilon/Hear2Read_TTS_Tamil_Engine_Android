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
/*  indic language support                                            */
/*                                                                       */
/*************************************************************************/
#include "flite.h"
#include "cst_val.h"
#include "cst_voice.h"
#include "cst_lexicon.h"
#include "cst_ffeatures.h"
#include "cmu_indic_lang.h"

/* ./bin/compile_regexes cst_rx_not_indic "^[0-9a-zA-Z/:_'-,]+$" */
static const unsigned char cst_rx_not_indic_rxprog[] = {
   156, 6, 0, 87, 1, 0, 3, 11, 0, 78, 4, 0, 0, 48, 49, 50, 
   51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102, 103, 104, 105, 
   106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 
   122, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 47, 58, 95, 39, 40, 
   41, 42, 43, 44, 0, 2, 0, 3, 0, 0, 0, 
};
static const cst_regex cst_rx_not_indic_rx = {
   0, 1, NULL, 0, 91,
   (char *)cst_rx_not_indic_rxprog
};
const cst_regex * const cst_rx_not_indic = &cst_rx_not_indic_rx;

/* ./bin/compile_regexes cst_rx_indic_eng_number "^[1-9][0-9],\\([0-9][1-9],\\)*[0-9][0-9][0-9]$" */
static const unsigned char cst_rx_indic_eng_number_rxprog[] = {
   156, 6, 0, 136, 1, 0, 3, 4, 0, 13, 49, 50, 51, 52, 53, 54, 
   55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
   57, 0, 8, 0, 5, 44, 0, 6, 0, 47, 21, 0, 3, 6, 0, 35, 
   4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 4, 0, 
   13, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 8, 0, 5, 44, 0, 
   31, 0, 3, 7, 0, 44, 6, 0, 3, 9, 0, 3, 4, 0, 14, 48, 
   49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 
   51, 52, 53, 54, 55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 51, 52, 
   53, 54, 55, 56, 57, 0, 2, 0, 3, 0, 0, 0, 
};
static const cst_regex cst_rx_indic_eng_number_rx = {
   0, 1, NULL, 0, 140,
   (char *)cst_rx_indic_eng_number_rxprog
};
const cst_regex * const cst_rx_indic_eng_number = &cst_rx_indic_eng_number_rx;

cst_val *us_tokentowords(cst_item *token);

/* Note that's an ascii | not the devangari one */
const cst_string * const indic_postpunctuationsymbols = "\"'`.,:;!?(){}[]|";

static cst_val *cmu_indic_tokentowords_one(cst_item *token, const char *name);
cst_val *cmu_indic_tokentowords(cst_item *token) {
  return cmu_indic_tokentowords_one(token, item_feat_string(token, "name"));
}

/* Indic numbers.  This deals with all (quantity) numbers found in any Indic */
/* language no matter what script they are written in.  We use the Indic_Nums */
/* table to convert the strings of digits (points and commas) into lists of */
/* words for those scripts' language.  This telugu digits get converted to  */
/* telugu words (even if the voice is a Hindi voice).                       */
/* We assume use lakh and crore examples when there is commas to identify   */
/* This 10,34,123 (in English digits) will be expanded to 10 lakh, thirty   */
/* four thousand one hundred (and) twenty three                             */

/* We do English too, so I can debug it, and so lakh and crore are right */
#include "indic_eng_num_table.h"
#include "indic_hin_num_table.h"
#if 0
#include "indic_tel_num_table.h"
#include "indic_mar_num_table.h"
#include "indic_guj_num_table.h"
#endif

static cst_val *indic_number_digit(const char *digit,const indic_num_table *t)
{
    int i;

    if ((digit == NULL) || (t == NULL))
        return NULL;

    for (i=0; t->digit[i][0] != NULL; i++)
        if (cst_streq(digit,num_table_digit(t,i,0)))
            return cons_val(string_val(num_table_digit(t,i,1)),NULL);

    return NULL;
}

static cst_val *indic_number_two_digit(const char *digit1,
                                       const char *digit2,
                                       const indic_num_table *t)
{
    int i,j;
    cst_val *r = NULL;

    if ((digit1 == NULL) || (digit2 == NULL) || (t == NULL))
        return NULL;

    for (i=0; num_table_two_digit(t,i,0) != NULL; i+=10)
    {
        if (cst_streq(digit1,num_table_two_digit(t,i,0)))
        {
            for (j=0; j<10; j++)
                if (cst_streq(digit2,num_table_two_digit(t,i+j,1)))
                {
                    r = NULL;
                    if (num_table_two_digit(t,i+j,3) != NULL)
                        r = cons_val(string_val(num_table_two_digit(t,i+j,3)),r);
                    if (num_table_two_digit(t,i+j,2) != NULL)
                        r = cons_val(string_val(num_table_two_digit(t,i+j,2)),r);
                    return r;
                }
            return r;
        }
    }

    return r;
}

static cst_val *indic_number_hundred(const indic_num_table *num_table)
{
    return string_val(num_table->hundred);
}
static cst_val *indic_number_thousand(const indic_num_table *num_table)
{
    return string_val(num_table->thousand);
}
static cst_val *indic_number_lakh(const indic_num_table *num_table)
{
    return string_val(num_table->lakh);
}
static cst_val *indic_number_crore(const indic_num_table *num_table)
{
    return string_val(num_table->crore);
}

cst_val *indic_number(const cst_val *number,
                      const indic_num_table *num_table)
{
    cst_val *r = NULL;
    /* so its a number in some script (we actually don't care which script) */

#if 1
    printf("awb_debug enter indic num ");
    val_print(stdout,number); printf("\n");
#endif

    if (number == NULL)
        r = NULL;
    else if (((cst_streq(val_string(val_car(number)),
                        num_table_digit(num_table,0,0))) ||
              (cst_streq(val_string(val_car(number)),"0"))) &&
             (val_cdr(number) != NULL))
        /* Is there a leading zero (native or English) */
        r = indic_number(val_cdr(number),num_table);
    else if (val_length(number) == 1)
        r = indic_number_digit(val_string(val_car(number)),num_table);
    else if (val_length(number) == 2)
        r = indic_number_two_digit(val_string(val_car(number)),
                                     val_string(val_car(val_cdr(number))),
                                     num_table);
    else if (val_length(number) == 3)
    {
        r = val_append(indic_number_digit(val_string(val_car(number)),num_table),
                 cons_val(indic_number_hundred(num_table),
                          indic_number(val_cdr(number),num_table)));
    }
    else if (val_length(number) == 4)
    {
        r = val_append(indic_number_digit(val_string(val_car(number)),num_table),
                 cons_val(indic_number_thousand(num_table),
                          indic_number(val_cdr(number),num_table)));
    }
    else if (val_length(number) == 5)
    {
        r = val_append(indic_number_two_digit(val_string(val_car(number)),
                                       val_string(val_car(val_cdr(number))),
                                       num_table),
                 cons_val(indic_number_thousand(num_table),
                          indic_number(val_cdr(val_cdr(number)),num_table)));
    }
    else if (val_length(number) == 6)
    {
        r = val_append(indic_number_digit(val_string(val_car(number)),num_table),
                 cons_val(indic_number_lakh(num_table),
                          indic_number(val_cdr(number),num_table)));
    }
    else if (val_length(number) == 7)
    {
        r = val_append(indic_number_two_digit(val_string(val_car(number)),
                                       val_string(val_car(val_cdr(number))),
                                       num_table),
                       cons_val(indic_number_lakh(num_table),
                          indic_number(val_cdr(val_cdr(number)),num_table)));
    }
    else if (val_length(number) == 8)
    {
        r = val_append(indic_number_digit(val_string(val_car(number)),num_table),
                 cons_val(indic_number_crore(num_table),
                          indic_number(val_cdr(number),num_table)));
    }
    else if (val_length(number) == 9)
    {
        r = val_append(indic_number_two_digit(val_string(val_car(number)),
                                        val_string(val_car(val_cdr(number))),
                                       num_table),
                 cons_val(indic_number_crore(num_table),
                          indic_number(val_cdr(val_cdr(number)),num_table)));
    }
    
#if 1
    printf("awb_debug end of indic num ");
    val_print(stdout,r); printf("\n");
#endif

    return r;
}

static int indic_nump(const char *number,
                      const indic_num_table *num_table)
{
    /* True if all (unicode) characters are in num_table's digit table */
    /* or is a comma or dot */
    cst_val *p;
    const cst_val *q;
    int i;
    int flag = TRUE;
    int fflag;

    p = cst_utf8_explode(number);
    for (q=p; q && (flag==TRUE); q=val_cdr(q))
    {
        fflag = FALSE;
        for (i=0; i<10; i++)
        {
            if (cst_streq(val_string(val_car(q)),
                          num_table_digit(num_table,i,0)))
            {
                fflag = TRUE;
                break;
            }
        }
        if ((cst_streq(val_string(val_car(q)),",")) ||
            /* English zeros sometimes occur */
            (cst_streq(val_string(val_car(q)),"0")))
            fflag = TRUE;
        flag = fflag;
    }

    return flag;

}

static cst_val *indic_num_normalize(const char *number,
                                    const indic_num_table *num_table)
{
    /* Remove , (dot?) and map '0' to native 0 */
    cst_val *p, *np;
    const cst_val *q;

    p = cst_utf8_explode(number);
    np = NULL;
    for (q=p; q; q=val_cdr(q))
    {
        if (cst_streq(val_string(val_car(q)),"0"))
            np = cons_val(string_val(num_table_digit(num_table,0,0)),np);
        else if (!cst_streq(val_string(val_car(q)),","))
            np = cons_val(string_val(val_string(val_car(q))),np);
    }
    delete_val(p);
    return val_reverse(np);
}

static cst_val *cmu_indic_tokentowords_one(cst_item *token, const char *name)
{
    /* Return list of words that expand token/name */
    cst_val *r, *p;
    const indic_num_table *num_table;
    const char *variant;
    cst_utterance *utt;

    /*    printf("token_name %s name %s\n",item_name(token),name); */

    if (item_feat_present(token,"phones"))
	return cons_val(string_val(name),NULL);

#if 0
    if (item_feat_present(token,"nsw"))
	nsw = item_feat_string(token,"nsw");

    utt = item_utt(token);
    lex = val_lexicon(feat_val(utt->features,"lexicon"));
#endif
    utt = item_utt(token);
    variant = get_param_string(utt->features, "variant", "none");
    if (cst_streq(variant,"hin"))
        num_table = &hin_num_table;
    else
        num_table = &eng_num_table;

    /* This matches *English* numbers of the form 99,99,999 that require lakh
       or crore expansion -- otherwise they'll be dropped back to the English
       front end */
    if (cst_regex_match(cst_rx_indic_eng_number,name))
    {
        num_table = &eng_num_table; /* should choose right table */
        /* remove commas */
        p = indic_num_normalize(name,num_table);
        r = indic_number(p,num_table);
        delete_val(p);
    }
    else if ((indic_nump(name,num_table)) &&
             (!cst_streq("0",name)))
            
    {   /* Its script specific digits (commas/dots) */
        p = indic_num_normalize(name,num_table);
        r = indic_number(p,num_table);
        delete_val(p);
    }
    else if (cst_regex_match(cst_rx_not_indic,name))
        /* Do English analysis on non-unicode tokens */
        r = us_tokentowords(token);
    else if (cst_strlen(name) > 0)
        r = cons_val(string_val(name),0);
    else
        r = NULL;

    return r;
}

int indic_utt_break(cst_tokenstream *ts,
                    const char *token,
                    cst_relation *tokens)
{
  const char *postpunct = item_feat_string(relation_tail(tokens), "punc");
  const char *ltoken = item_name(relation_tail(tokens));

  if (cst_strchr(ts->whitespace,'\n') != cst_strrchr(ts->whitespace,'\n'))
    /* contains two new lines */
    return TRUE;
  else if ((cst_strlen(ltoken) >= 3) &&
           (cst_streq(&ltoken[cst_strlen(ltoken)-3],"।"))) /* devanagari '|' */
      return TRUE;
  else if (strchr(postpunct,':') ||
           strchr(postpunct,'?') ||
           strchr(postpunct,'|') ||  /* if ascii '|' gets used as dvngr '|' */
           strchr(postpunct,'!'))
    return TRUE;
  else if (strchr(postpunct,'.'))
    return TRUE;
  else
    return FALSE;
}

void cmu_indic_lang_init(cst_voice *v)
{
    /* Set indic language stuff */
    feat_set_string(v->features,"language","cmu_indic_lang");

    /* utterance break function */
    feat_set(v->features,"utt_break",breakfunc_val(&indic_utt_break));

    /* Phoneset -- need to get this from voice */
    feat_set(v->features,"phoneset",phoneset_val(&cmu_indic_phoneset));
    feat_set_string(v->features,"silence",cmu_indic_phoneset.silence);

    /* Get information from voice and add to lexicon */

    /* Text analyser -- whitespace defaults */
    feat_set_string(v->features,"text_whitespace",
                    cst_ts_default_whitespacesymbols);
    feat_set_string(v->features,"text_prepunctuation",
                    cst_ts_default_prepunctuationsymbols);
    /* We can't put multi-byte characters in these classes so we can't */
    /* add devanagari end of sentence '|' here, but would like to --   */
    /* But we do add ascii '|' to it as it sometimes gets used the same way */
    feat_set_string(v->features,"text_postpunctuation",
                    indic_postpunctuationsymbols);
    feat_set_string(v->features,"text_singlecharsymbols",
                    cst_ts_default_singlecharsymbols);

    /* Tokenization tokenization function */
    feat_set(v->features,"tokentowords_func",itemfunc_val(&cmu_indic_tokentowords));
    /* Pos tagger (gpos)/induced pos */

    /* Phrasing */
    feat_set(v->features,"phrasing_cart",cart_val(&cmu_indic_phrasing_cart));
        
    /* Intonation, Duration and F0 -- part of cg */
    feat_set_string(v->features,"no_intonation_accent_model","1");

    /* Default ffunctions (required) */
    basic_ff_register(v->ffunctions);

    return;
}
