/*
    Copyright (C) 1995-1998 AROS - The Amiga Research OS
    $Id$

    Desc: InitLocale() load locale preferences from a file.
    Lang: english
*/

#include <exec/types.h>
#include "locale_intern.h"
#include <libraries/iffparse.h>
#include <prefs/prefhdr.h>
#include <prefs/locale.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <aros/asmcall.h>

extern AROS_UFP4(ULONG, AROS_SLIB_ENTRY(strcompare, english), 
    AROS_UFPA(STRPTR, string1, A0),
    AROS_UFPA(STRPTR, string2, A1),
    AROS_UFPA(ULONG, length, D0),
    AROS_UFPA(ULONG, how, D1)
);
extern void *__eng_functable[];

#define EC(x)\
{\
    (x) = (((x) & 0xFF000000) >> 24)\
       |  (((x) & 0x00FF0000) >> 8)\
       |  (((x) & 0x0000FF00) << 8)\
       |  (((x) & 0x000000FF) << 24);\
}

/*
    BOOL SetLocaleLanguage(struct IntLocale *, struct LocaleBase *)

    Try and set up the language of a Locale structure.
*/

void SetLocaleLanguage(struct IntLocale *il, struct LocaleBase *LocaleBase)
{
    struct Library *lang = NULL;
    ULONG mask = 0;
    UBYTE fileBuf[512];
    int i = 0;

    while(lang == NULL && i < 10)
    {
	STRPTR lName = il->il_Locale.loc_PrefLanguages[i];

	/* Is this english? If not try and load the language */
	if(
    	    AROS_UFC4(ULONG, AROS_SLIB_ENTRY(strcompare, english),
		AROS_UFCA(STRPTR, "english.", A0),
		AROS_UFCA(STRPTR, lName, A1),
		AROS_UFCA(ULONG, 10, D0),
		AROS_UFCA(ULONG, SC_ASCII, D1)) != 0)
	{
    	    /* Try and open the specified language */
    	    lang = OpenLibrary(lName, 0);

    	    if(lang == NULL)
    	    {
		/*
    		    Ok, so the language didn't open, lets try for
    		    LOCALE:Languages/xxx.language
		*/
		CopyMem("LOCALE:Languages/", fileBuf, 18);
		AddPart(fileBuf, FilePart(lName), 512);

		lang = OpenLibrary(fileBuf, 0);
	    }
    	    if(lang == NULL)
    	    {
		/*
    		    Ok, so we are still NULL, lets then try for
    		    PROGDIR:Languages/xxx.language
		*/
		CopyMem("PROGDIR:Languages/", fileBuf, 19);
		AddPart(fileBuf, FilePart(lName), 512);
	    }
    	    /* If it is still no good, then we have no hope */
	}
	i++;
    }

    /*
	Ok so we now should have a language, or nothing. Either way
	we now fill in the locale functions in the structure.
	I remember there was a VERY big bug in here once, which I 
	finally managed to fix, only to blow away the only copy
	of the file (and the rest of the locale.library) from all
	existance. Sigh.

	If we could open any of the libraries, replace its function,
	otherwise replace none of them.
    */

    il->il_CurrentLanguage = lang;

    if(lang != NULL)
    {
	mask = AROS_LC0(ULONG, mask, struct Library *, lang, 5, Language);
    	il->il_Locale.loc_LanguageName =
	    il->il_Locale.loc_PrefLanguages[i-1];
    }
    else
	mask = 0;

    /* CONST: If we add any more functions we need to increase this number */   
    for(i=0; i < 17; i++)
    {
	if(mask & (1<<i))
	{
	    il->il_LanguageFunctions[i] = __AROS_GETVECADDR(lang, (i+6));
	}
	else
	{
	    il->il_LanguageFunctions[i] = __eng_functable[i];
	}
    }
}

/* InitLocale(IntLocale *, LocalePrefs *)
    This procedure will extract all the relevant information out of
    a LocalePrefs structure, and stuff it into a IntLocale structure.

    This is really a silly way of doing things, but unfortunately it
    is the only way that doesn't keep the entire chunk in memory,
    (by duplicating the LocalePrefs chunk), or by doing extra loops
    on things (counting string sizes, then allocating).

    This puts it smack bang in the centre of speed/memory use.
    This is mainly a separate procedure to get rid of the indentation.
*/

void InitLocale(struct IntLocale *locale, struct LocalePrefs *lp, struct LocaleBase *LocaleBase) 
{
    struct CountryPrefs *cp;
    int i;

    cp = &lp->lp_CountryData;

    /*
	We can copy 300 bytes straight away since
	the prefered languages are all in a row.
    */
    CopyMem(lp->lp_PreferredLanguages[0],
	    locale->PreferredLanguages[0], 300);

    for(i=0; i < 10; i++)
    {
	locale->il_Locale.loc_PrefLanguages[i] = locale->PreferredLanguages[i];
    }

    /*
	If we cannot open ANY of the languages, then all the language
	function vectors would have the default language data.
    */
    SetLocaleLanguage(locale, LocaleBase);

    locale->il_Locale.loc_Flags = 0;
    locale->il_Locale.loc_CodeSet = 0;
    locale->il_Locale.loc_CountryCode = cp->cp_CountryCode;
    locale->il_Locale.loc_TelephoneCode = cp->cp_TelephoneCode;
    locale->il_Locale.loc_GMTOffset = lp->lp_GMTOffset;
    locale->il_Locale.loc_CalendarType = cp->cp_CalendarType;

#if (AROS_BIG_ENDIAN == 0)
    EC(locale->il_Locale.loc_CountryCode);
    EC(locale->il_Locale.loc_TelephoneCode);
    EC(locale->il_Locale.loc_GMTOffset);
#endif

    /* Another really large section to copy,
	from cp_DateTimeFormat[] to cp_MonFracGrouping[] incl
	80 + 40 + 40 + 80 + 40 + 40 + (10 * 10)
    */

    CopyMem(&cp->cp_DateTimeFormat[0], &locale->DateTimeFormat[0], 420);

    locale->il_Locale.loc_DateTimeFormat = &locale->DateTimeFormat[0];
    locale->il_Locale.loc_DateFormat = &locale->DateFormat[0];
    locale->il_Locale.loc_TimeFormat = &locale->TimeFormat[0];
    locale->il_Locale.loc_ShortDateTimeFormat = &locale->ShortDateTimeFormat[0];
    locale->il_Locale.loc_ShortDateFormat = &locale->ShortDateFormat[0];
    locale->il_Locale.loc_ShortTimeFormat = &locale->ShortTimeFormat[0];

    locale->il_Locale.loc_DecimalPoint = &locale->DecimalPoint[0];
    locale->il_Locale.loc_GroupSeparator = &locale->GroupSeparator[0];
    locale->il_Locale.loc_FracGroupSeparator = &locale->FracGroupSeparator[0];
    locale->il_Locale.loc_Grouping = &locale->Grouping[0];
    locale->il_Locale.loc_FracGrouping = &locale->FracGrouping[0];

    locale->il_Locale.loc_MonDecimalPoint = &locale->MonDecimalPoint[0];
    locale->il_Locale.loc_MonGroupSeparator = &locale->MonGroupSeparator[0];
    locale->il_Locale.loc_MonFracGroupSeparator = &locale->MonFracGroupSeparator[0];
    locale->il_Locale.loc_MonGrouping = &locale->MonGrouping[0];
    locale->il_Locale.loc_MonFracGrouping = &locale->MonFracGrouping[0];

    locale->il_Locale.loc_MonFracDigits = cp->cp_MonFracDigits;
    locale->il_Locale.loc_MonIntFracDigits = cp->cp_MonIntFracDigits;

    /* The three currency symbols, and +ve sign */
    CopyMem(&cp->cp_MonCS, &locale->MonCS[0], 40);

    locale->il_Locale.loc_MonCS = &locale->MonCS[0];
    locale->il_Locale.loc_MonSmallCS = &locale->MonSmallCS[0];
    locale->il_Locale.loc_MonIntCS = &locale->MonIntCS[0];
    locale->il_Locale.loc_MonPositiveSign = &locale->MonPositiveSign[0];

    locale->il_Locale.loc_MonPositiveSpaceSep = cp->cp_MonPositiveSpaceSep;
    locale->il_Locale.loc_MonPositiveSignPos = cp->cp_MonPositiveSignPos;
    locale->il_Locale.loc_MonPositiveCSPos = cp->cp_MonPositiveCSPos;

    CopyMem(&cp->cp_MonNegativeSign[0], &locale->MonNegativeSign[0], 10);
    locale->il_Locale.loc_MonNegativeSign = &locale->MonNegativeSign[0];
    locale->il_Locale.loc_MonNegativeSpaceSep = cp->cp_MonNegativeSpaceSep;
    locale->il_Locale.loc_MonNegativeSignPos = cp->cp_MonNegativeSignPos;
    locale->il_Locale.loc_MonNegativeCSPos = cp->cp_MonNegativeCSPos;
}

