# vim: ft=sh

# Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)

# This is a shell include file. It should not be executed!

# Each of these arrays are filled with filenames correct to the convention the array name
# specifies. The first member of each array will become a directory, and all following members
# will become files. This first member (the directory) will be the location where recursive
# search tests will be placed. Following this convention, we can simply re-create files and 
# directories from the array inside of the first member we just made.

#----------------------------------------------------------------------------------------------#
# Test Filenames
#----------------------------------------------------------------------------------------------#
# Camelcase.
camelcase_lenient_matches=(
"camel"
".camelcase"
"camelcase.txt"
"camelcase.cfg"
"camelcase.o"
"camelcase.a"
"camelcasetestfile.txt"
"camelcase.RAR"
".camelcase.JPEG"
"superlongcamelcasetestfilethatjustseemstokeepongoingandgoingandgoingandgoing.bat"
)

camelcase_strict_matches=(
"camelCase"
".camelCase"
"camelCase.txt"
"camelCase000000.cfg"
"camelCase12345.o"
"camelCase123.a"
"camelCaseTestFile.txt"
"camelCase123.RAR"
".camelC4se.JPEG"
"superLongCamelCaseTestFileThatJustSeemsToKeepOnGoingAndGoingAndGoingAndGoing.bat"
)


# Cobolcase.
cobolcase_lenient_matches=(
"COBOL"
".COBOLCASE"
"COBOLCASETEST.txt"
"COBOLCASETEST.cfg"
"COBOLCASETEST.o"
"COBOLCASETEST.a"
"COBOLCASETESTFILE.txt"
"COBOLCASETEST.RAR"
".COBOLCASETEST.JPEG"
"SUPERLONGCOBOLCASETESTFILETHATJUSTSEEMSTOKEEPONGOINGANDGOINGANDGOINGANDGOING.bat"
)

cobolcase_strict_matches=(
"COBOL-CASE"
".COBOL-CASE"
"COBOL-CASE-TEST.txt"
"COBOL-CASE-TEST-000000.cfg"
"COBOL-CASE-TEST12345.o"
"COBOL-CASE-TEST123.a"
"COBOL-CASE-TEST-FILE.txt"
"COBOL-CASE-TEST-123.RAR"
".COBOL-C4SE-TEST.JPEG"
"SUPER-LONG-COBOL-CASE-TEST-FILE-THAT-JUST-SEEMS-TO-KEEP-ON-GOING-AND-GOING-AND-GOING-AND-GOING.bat"
)


# Constantcase.
constantcase_lenient_matches=(
"CONSTANTCASE"
".CONSTANTCASE"
"CONSTANTCASETEST.txt"
"CONSTANTCASETEST.cfg"
"CONSTANTCASETEST.o"
"CONSTANTCASETEST.a"
"CONSTANTCASETESTFILE.txt"
"CONSTANTCASETEST.RAR"
".CONSTANTCASETEST.JPEG"
"SUPERLONGCONSTANTCASETESTFILETHATJUSTSEEMSTOKEEPONGOINGANDGOINGANDGOINGANDGOING.bat"
)

constantcase_strict_matches=(
"CONSTANT_CASE"
".CONSTANT_CASE"
"CONSTANT_CASE_TEST.txt"
"CONSTANT_CASE_TEST_000000.cfg"
"CONSTANT_CASE_TEST12345.o"
"CONSTANT_CASE_TEST123.a"
"CONSTANT_CASE_TEST_FILE.txt"
"CONSTANT_CASE_TEST_123.RAR"
".CONST4NT_C4SE_TEST.JPEG"
"SUPER_LONG_CONSTANT_CASE_TEST_FILE_THAT_JUST_SEEMS_TO_KEEP_ON_GOING_AND_GOING_AND_GOING_AND_GOING.bat"
)


# Flatcase.
flatcase_lenient_matches=(
"flatcase"
".flatcase"
"flatcase.txt"
"flatcase000000.cfg"
"12345flatcase"
"flat123case123.a"
"flatcase.c3"
"flatcase123.RAR"
"123flatcase.JPEG"
"superlongflatcasetestfilethatjustseemstokeepongoingandgoingandgoingandgoing.bat"
)

flatcase_strict_matches=(
"flatcase"
".flatcase"
"flatcase.txt"
"flatcase000000.cfg"
"12345flatcase"
"flat123case123.a"
"flatcase.c3"
"flatcase123.RAR"
"123flatcase.JPEG"
"superlongflatcasetestfilethatjustseemstokeepongoingandgoingandgoingandgoing.bat"
)


# Kebabcase.
kebabcase_lenient_matches=(
"kebabcase"
".kebabcase"
"kebabcasetest.txt"
"kebabcasetest.cfg"
"kebabcasetest.o"
"kebabcasetest.a"
"kebabcasetestfile.txt"
"kebabcasetest.RAR"
".kebabcasetest.JPEG"
"superlongkebabcasetestfilethatjustseemstokeepongoingandgoingandgoingandgoing.bat"
)

kebabcase_strict_matches=(
"kebab-case"
".kebab-case"
"kebab-case-test.txt"
"kebab-case-test-000000.cfg"
"kebab-case-test12345.o"
"kebab-case-test123.a"
"kebab-case-test-file.txt"
"kebab-case-test-123.RAR"
".keb4b-c4se-test.JPEG"
"super-long-kebab-case-test-file-that-just-seems-to-keep-on-going-and-going-and-going-and-going.bat"
)


# Pascalcase.
pascalcase_lenient_matches=(
"PascalCase"
".PascalCase"
"PascalCaseTest.txt"
"PascalCaseTest000000.cfg"
"PascalCaseTest12345.o"
"PascalCaseTest123.a"
"PascalCaseTestFile.txt"
"PascalCaseTest123.RAR"
".P4scalC4seTest.JPEG"
"SuperLongPascalCaseTestFileThatJustSeemsToKeepOnGoingAndGoingAndGoingAndGoing.bat"
)

pascalcase_strict_matches=(
"PascalCase"
".PascalCase"
"PascalCaseTest.txt"
"PascalCaseTest000000.cfg"
"PascalCaseTest12345.o"
"PascalCaseTest123.a"
"PascalCaseTestFile.txt"
"PascalCaseTest123.RAR"
".P4scalC4seTest.JPEG"
"SuperLongPascalCaseTestFileThatJustSeemsToKeepOnGoingAndGoingAndGoingAndGoing.bat"
)


# Snakecase.
snakecase_lenient_matches=(
"snakecase"
".snakecase"
"snakecasetest.txt"
"snakecasetest.cfg"
"snakecasetest.o"
"snakecasetest.a"
"snakecasetestfile.txt"
"snakecasetest.RAR"
".snakecasetest.JPEG"
"superlongsnakecasetestfilethatjustseemstokeepongoingandgoingandgoingandgoing.bat"
)

snakecase_strict_matches=(
"snake_case"
".snake_case"
"snake_case_test.txt"
"snake_case_test_000000.cfg"
"snake_case_test12345.o"
"snake_case_test123.a"
"snake_case_test_file.txt"
"snake_case_test_123.RAR"
".sn4ke_c4se_test.JPEG"
"super_long_snake_case_test_file_that_just_seems_to_keep_on_going_and_going_and_going_and_going.bat"
)
