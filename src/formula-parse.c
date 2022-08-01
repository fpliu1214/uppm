#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <yaml.h>
#include "uppm.h"

typedef enum {
    FORMULA_KEY_CODE_unknown,
    FORMULA_KEY_CODE_summary,
    FORMULA_KEY_CODE_webpage,
    FORMULA_KEY_CODE_version,
    FORMULA_KEY_CODE_license,
    FORMULA_KEY_CODE_git_url,
    FORMULA_KEY_CODE_bin_url,
    FORMULA_KEY_CODE_bin_sha,
    FORMULA_KEY_CODE_dep_pkg,
    FORMULA_KEY_CODE_install
} UPPMFormulaKeyCode;

void uppm_formula_dump(UPPMFormula * formula) {
    if (formula != NULL) {
        printf("summary: %s\n", formula->summary);
        printf("webpage: %s\n", formula->webpage);
        printf("version: %s\n", formula->version);
        printf("license: %s\n", formula->license);
        printf("git_url: %s\n", formula->git_url);
        printf("bin_url: %s\n", formula->bin_url);
        printf("bin_sha: %s\n", formula->bin_sha);
        printf("dep_pkg: %s\n", formula->dep_pkg);
        printf("install: %s\n", formula->install);
    }
}

void uppm_formula_free(UPPMFormula * formula) {
    if (formula != NULL) {
        if (formula->summary != NULL) {
            free(formula->summary);
            formula->summary = NULL;
        }

        if (formula->webpage != NULL) {
            free(formula->webpage);
            formula->webpage = NULL;
        }

        if (formula->version != NULL) {
            free(formula->version);
            formula->version = NULL;
        }

        if (formula->license != NULL) {
            free(formula->license);
            formula->license = NULL;
        }

        if (formula->git_url != NULL) {
            free(formula->git_url);
            formula->git_url = NULL;
        }

        if (formula->bin_url != NULL) {
            free(formula->bin_url);
            formula->bin_url = NULL;
        }

        if (formula->bin_sha != NULL) {
            free(formula->bin_sha);
            formula->bin_sha = NULL;
        }

        if (formula->dep_pkg != NULL) {
            free(formula->dep_pkg);
            formula->dep_pkg = NULL;
        }

        if (formula->install != NULL) {
            free(formula->install);
            formula->install = NULL;
        }

        free(formula);
        formula = NULL;
    }
}

static UPPMFormulaKeyCode uppm_formula_key_code_from_key_name(char * key) {
           if (strcmp(key, "summary") == 0) {
        return FORMULA_KEY_CODE_summary;
    } else if (strcmp(key, "webpage") == 0) {
        return FORMULA_KEY_CODE_webpage;
    } else if (strcmp(key, "version") == 0) {
        return FORMULA_KEY_CODE_version;
    } else if (strcmp(key, "license") == 0) {
        return FORMULA_KEY_CODE_license;
    } else if (strcmp(key, "git-url") == 0) {
        return FORMULA_KEY_CODE_git_url;
    } else if (strcmp(key, "bin-url") == 0) {
        return FORMULA_KEY_CODE_bin_url;
    } else if (strcmp(key, "bin-sha") == 0) {
        return FORMULA_KEY_CODE_bin_sha;
    } else if (strcmp(key, "dep-pkg") == 0) {
        return FORMULA_KEY_CODE_dep_pkg;
    } else if (strcmp(key, "install") == 0) {
        return FORMULA_KEY_CODE_install;
    } else {
        return FORMULA_KEY_CODE_unknown;
    }
}

void uppm_formula_set_value(UPPMFormulaKeyCode keyCode, char * value, UPPMFormula * formula) {
    value = strdup(value);
    switch (keyCode) {
        case FORMULA_KEY_CODE_summary:  formula->summary = value; break;
        case FORMULA_KEY_CODE_webpage:  formula->webpage = value; break;
        case FORMULA_KEY_CODE_version:  formula->version = value; break;
        case FORMULA_KEY_CODE_license:  formula->license = value; break;
        case FORMULA_KEY_CODE_git_url:  formula->git_url = value; break;
        case FORMULA_KEY_CODE_bin_url:  formula->bin_url = value; break;
        case FORMULA_KEY_CODE_bin_sha:  formula->bin_sha = value; break;
        case FORMULA_KEY_CODE_dep_pkg:  formula->dep_pkg = value; break;
        case FORMULA_KEY_CODE_install:  formula->install = value; break;
        default: break;
    }
}

int uppm_formula_parse(const char * filepath, UPPMFormula * * out) {
    if ((filepath == NULL) || (strcmp(filepath, "") == 0)) {
        fprintf(stderr, "%s\n", "filepath is not given.");
        return UPPM_ARG_INVALID;
    }

    FILE * file = fopen(filepath, "r");

    if (file == NULL) {
        perror(filepath);
        return UPPM_ERROR;
    }

    yaml_parser_t parser;
    yaml_token_t  token;

    // https://libyaml.docsforge.com/master/api/yaml_parser_initialize/
    if (yaml_parser_initialize(&parser) == 0) {
        perror("Failed to initialize yaml parser");
        return UPPM_ERROR;
    }

    yaml_parser_set_input_file(&parser, file);

    UPPMFormulaKeyCode formulaKeyCode = FORMULA_KEY_CODE_unknown;

    UPPMFormula * formula = NULL;

    int lastTokenType = 0;

    bool success = true;

    do {
        // https://libyaml.docsforge.com/master/api/yaml_parser_scan/
        if (yaml_parser_scan(&parser, &token) == 0) {
            fprintf(stderr, "syntax error: %s\n", filepath);
            success = false;
            goto clean;
        }

        switch(token.type) {
            case YAML_KEY_TOKEN:
                lastTokenType = 1;
                break;
            case YAML_VALUE_TOKEN:
                lastTokenType = 2;
                break;
            case YAML_SCALAR_TOKEN:
                if (lastTokenType == 1) {
                    formulaKeyCode = uppm_formula_key_code_from_key_name((char*)token.data.scalar.value);
                } else if (lastTokenType == 2) {
                    if (formula == NULL) {
                        formula = (UPPMFormula*)calloc(1, sizeof(UPPMFormula));
                    }
                    uppm_formula_set_value(formulaKeyCode, (char*)token.data.scalar.value, formula);
                }
                break;
            default: 
                lastTokenType = 0;
                break;
        }

        if (token.type != YAML_STREAM_END_TOKEN) {
            yaml_token_delete(&token);
        }
    } while(token.type != YAML_STREAM_END_TOKEN);

clean:
    yaml_token_delete(&token);

    yaml_parser_delete(&parser);

    fclose(file);

    if (success) {
        if ((formula->bin_url == NULL) || (strcmp(formula->bin_url, "") == 0)) {
            fprintf(stderr, "bin-url not configed in %s\n", filepath);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if ((formula->bin_sha == NULL) || (strcmp(formula->bin_url, "") == 0)) {
            fprintf(stderr, "bin-sha not configed in %s\n", filepath);
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        if (strlen(formula->bin_sha) != 64) {
            fprintf(stderr, "value of bin-sha length must be 64\n");
            uppm_formula_free(formula);
            return UPPM_ERROR;
        }

        (*out) = formula;
        return UPPM_OK;
    } else {
        if (formula != NULL) {
            uppm_formula_free(formula);
        }
        return UPPM_ERROR;
    }
}
