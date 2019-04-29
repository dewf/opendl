import sys
import json

if len(sys.argv) != 2:
    print("usage: gen_bintray.py <build-tag>")
    sys.exit(-1)

build_tag = sys.argv[1]

out_desc = {
    "package": {
        "name": "OpenDL",
        "repo": "TeamGUI",
        "subject": "dewf",
        "vcs_url": "https://github.com/dewf/opendl.git",
        "licenses": ["MIT"],
        # "labels": ["cool", "awesome", "gorilla"],
        "public_download_numbers": True,
        "public_stats": True
    },

    "version": {
        "name": build_tag, #"OpenDL-%s" % (build_tag,),
        "desc": "OpenDL Mac CI Build",
        #"released": "2015-01-04",
        #"vcs_tag": "0.5",
        "gpgSign": False
    },

    "files":
        [
            {
                "includePattern": "OpenDL.dmg", 
                "uploadPattern": "OpenDL/%s/OpenDL-%s-Mac-Framework.dmg" % (build_tag, build_tag),
                "matrixParams":
                { 
                    "override": 1 
                }
            }
        ],
    "publish": True
}
with open("bintray.json", "w") as outfile:
    json.dump(out_desc, outfile, indent=4)


