#include "HTMLExporter.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

void HTMLExporter::exportReport(
    const std::vector<ArbitrageOpportunity>& opportunities,
    const std::string& filePath,
    size_t totalCycles,
    size_t profitableCycles) const {
    std::ofstream out(filePath);
    if (!out) {
        throw std::runtime_error("Failed to open output HTML file: " + filePath);
    }

    out << "<!DOCTYPE html>\n"
        << "<html lang=\"en\">\n"
        << "<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        << "  <title>DEX Arbitrage Report</title>\n"
        << "  <style>\n"
        << "    :root { --bg:#f6f7fb; --card:#ffffff; --ink:#132238; --muted:#5a6b82; --accent:#0b66ff; --good:#0f9d58; }\n"
        << "    body { margin:0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background:linear-gradient(120deg,#f6f7fb,#e8eefc); color:var(--ink); }\n"
        << "    .wrap { max-width:1200px; margin:2rem auto; padding:0 1rem; }\n"
        << "    .card { background:var(--card); border-radius:12px; box-shadow:0 12px 40px rgba(18,36,64,0.08); padding:1.2rem; }\n"
        << "    h1 { margin-top:0; }\n"
        << "    .meta { color:var(--muted); margin-bottom:1rem; }\n"
        << "    table { width:100%; border-collapse:collapse; font-size:0.92rem; }\n"
        << "    th, td { text-align:left; border-bottom:1px solid #e3e8f0; padding:0.6rem 0.4rem; vertical-align:top; }\n"
        << "    th { color:#20344f; background:#f1f5fd; position:sticky; top:0; }\n"
        << "    .profit { color:var(--good); font-weight:600; }\n"
        << "    .mono { font-family: Consolas, 'Courier New', monospace; font-size:0.82rem; }\n"
        << "  </style>\n"
        << "</head>\n"
        << "<body>\n"
        << "  <div class=\"wrap\">\n"
        << "    <div class=\"card\">\n"
        << "      <h1>Top Arbitrage Cycles</h1>\n"
        << "      <div class=\"meta\">Total cycles detected: " << totalCycles
        << " | Profitable cycles: " << profitableCycles << "</div>\n"
        << "      <table>\n"
        << "        <thead><tr>\n"
        << "          <th>Rank</th><th>Cycle Path</th><th>Pool IDs</th><th>Start Token</th><th>Optimal Trade Size</th><th>Profit (USD)</th><th>ROI (%)</th>\n"
        << "        </tr></thead>\n"
        << "        <tbody>\n";

    size_t rank = 1;
    for (const auto& opp : opportunities) {
        out << "          <tr>\n"
            << "            <td>" << rank++ << "</td>\n"
            << "            <td class=\"mono\">" << escapeHtml(formatCyclePath(opp.cycle)) << "</td>\n"
            << "            <td class=\"mono\">" << escapeHtml(formatPoolIds(opp.cycle)) << "</td>\n"
            << "            <td class=\"mono\">" << escapeHtml(opp.startTokenId) << "</td>\n"
            << "            <td>" << std::fixed << std::setprecision(8) << opp.optimalTradeSize << "</td>\n"
            << "            <td class=\"profit\">" << std::fixed << std::setprecision(4) << opp.profitUsd << "</td>\n"
            << "            <td>" << std::fixed << std::setprecision(4) << opp.percentageReturn << "</td>\n"
            << "          </tr>\n";
    }

    out << "        </tbody>\n"
        << "      </table>\n"
        << "    </div>\n"
        << "  </div>\n"
        << "</body>\n"
        << "</html>\n";
}

std::string HTMLExporter::escapeHtml(const std::string& s) const {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&#39;"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string HTMLExporter::formatCyclePath(const Cycle& cycle) const {
    std::ostringstream oss;
    for (size_t i = 0; i < cycle.tokenPath.size(); ++i) {
        if (i != 0) {
            oss << " -> ";
        }
        oss << cycle.tokenPath[i];
    }
    return oss.str();
}

std::string HTMLExporter::formatPoolIds(const Cycle& cycle) const {
    std::ostringstream oss;
    for (size_t i = 0; i < cycle.edges.size(); ++i) {
        if (i != 0) {
            oss << " | ";
        }
        oss << cycle.edges[i].pool->id();
    }
    return oss.str();
}
