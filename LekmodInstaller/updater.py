

import requests
import json
from datetime import datetime

class UpdateChecker:
    def __init__(self, config):
        self.config = config
        self.version_url = config.get('version_check_url')

    @staticmethod
    def catalog_releases(raw):

        if not isinstance(raw, dict):
            return {}
        releases = {}
        for key, info in raw.items():
            if not key or str(key).startswith('_'):
                continue
            if isinstance(info, dict) and info.get('file_id'):
                releases[key] = info
        return releases

    def get_available_versions(self, url=None, fallback_key='versions', allow_empty=False):

        versions = {}
        version_url = url if url is not None else self.version_url


        if version_url:
            try:

                import time
                cache_bust = f"?t={int(time.time())}"
                url_with_cache_bust = version_url + cache_bust

                response = requests.get(url_with_cache_bust, timeout=10)
                if response.status_code == 200:
                    online_versions = self.catalog_releases(response.json())
                    if online_versions or allow_empty:
                        return online_versions
                else:
                    print(f"GitHub returned status code: {response.status_code}")
            except Exception as e:
                print(f"Failed to fetch online versions: {e}")



        if fallback_key in self.config:
            versions.update(self.catalog_releases(self.config.get(fallback_key) or {}))

        if allow_empty:
            return versions


        if not versions:
            raise Exception("No versions available from GitHub or local config")

        return versions

    def compare_versions(self, v1, v2):

        def parse_version(v):

            v = v.lower().replace('v', '')
            parts = v.split('.')
            return [int(p) for p in parts if p.isdigit()]

        try:
            v1_parts = parse_version(v1)
            v2_parts = parse_version(v2)


            for i in range(max(len(v1_parts), len(v2_parts))):
                p1 = v1_parts[i] if i < len(v1_parts) else 0
                p2 = v2_parts[i] if i < len(v2_parts) else 0

                if p1 > p2:
                    return 1
                elif p1 < p2:
                    return -1

            return 0
        except:
            return 0

    def get_latest_version(self, versions):

        if not versions:
            return None

        version_list = list(versions.keys())
        version_list.sort(key=lambda v: self.parse_version_key(v), reverse=True)

        return version_list[0] if version_list else None

    def parse_version_key(self, version):

        try:
            v = version.lower().replace('v', '')
            parts = v.split('.')
            return tuple(int(p) for p in parts if p.isdigit())
        except:
            return (0,)

